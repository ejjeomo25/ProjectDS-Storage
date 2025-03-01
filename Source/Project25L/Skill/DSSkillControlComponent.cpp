//Default
#include "Skill/DSSkillControlComponent.h"

//UE
#include "Net/UnrealNetwork.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include "DSSkillBase.h"
#include "Character/DSCharacterBase.h"
#include "DSLogChannels.h"


UDSSkillControlComponent::UDSSkillControlComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	// PrimaryComponentTick.bStartWithTickEnabled = true; // FIXME! Just temp until timer manager figured out
	bAutoActivate = true;	// Forcing AutoActivate since above we manually force tick enabled.
	// if we don't have this, UpdateShouldTick() fails to have any effect
	// because we'll be receiving ticks but bIsActive starts as false

	bCachedIsNetSimulated = false;

	AffectedAnimInstanceTag = NAME_None;

	bSkillPendingClearAll = false;

}


void UDSSkillControlComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDSSkillControlComponent, ActivatableSkills);
}

void UDSSkillControlComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);

	SkillActorInfo = MakeShared<FDSSkillActorInfo>();

	AActor* Owner = GetOwner();
	InitSkillActorInfo(Owner, Owner);
}

void UDSSkillControlComponent::OnRegister()
{
	Super::OnRegister();

	ActivatableSkills.RegisterWithOwner(this);
}

void UDSSkillControlComponent::OnUnregister()
{
	Super::OnUnregister();
}

// ---------------------------------------------------------- FindSkillSpec ----------------------------------------------------------

FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromHandle(FDSSkillSpecHandle Handle) const
{
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.Handle == Handle)
		{
			return const_cast<FDSSkillSpec*>(&Spec);
		}
	}

	return nullptr;
}



FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromClass(TSubclassOf<UDSSkillBase> InAbilityClass) const
{
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.Skill == nullptr)
		{
			continue;
		}

		if (Spec.Skill->GetClass() == InAbilityClass)
		{
			return const_cast<FDSSkillSpec*>(&Spec);
		}
	}

	return nullptr;
}

FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromInputID(int32 InputID) const
{
	if (InputID != INDEX_NONE)
	{
		for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
		{
			if (Spec.InputID == InputID)
			{
				return const_cast<FDSSkillSpec*>(&Spec);
			}
		}
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------------------


UDSSkillBase* UDSSkillControlComponent::CreateNewInstanceOfSkill(FDSSkillSpec& Spec, const UDSSkillBase* Skill)
{
	check(Skill);
	check(Skill->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	UDSSkillBase* SkillInstance = NewObject<UDSSkillBase>(Owner, Skill->GetClass());
	check(SkillInstance);

	// Add it to one of our instance lists so that it doesn't GC.
	if (SkillInstance->GetReplicationPolicy() != ESkillReplicationPolicy::ReplicateNo)
	{
		Spec.ReplicatedInstances.Add(SkillInstance);
		AddReplicatedInstancedSkill(SkillInstance);
	}
	else
	{
		Spec.NonReplicatedInstances.Add(SkillInstance);
	}

	return SkillInstance;
}

void UDSSkillControlComponent::AddReplicatedInstancedSkill(UDSSkillBase* Skill)
{
	TArray<TObjectPtr<UDSSkillBase>>& ReplicatedAbilities = GetReplicatedInstancedSkills_Mutable();
	if (ReplicatedAbilities.Find(Skill) == INDEX_NONE)
	{
		ReplicatedAbilities.Add(Skill);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Skill);
		}
	}
}

void UDSSkillControlComponent::MarkSkillSpecDirty(FDSSkillSpec& Spec, bool WasAddOrRemove)
{
	if (IsOwnerActorAuthoritative())
	{
		// Don't mark dirty for specs that are server only unless it was an add/remove
		if (!(Spec.Skill && Spec.Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly && !WasAddOrRemove))
		{
			//DS_NETLOG(DSSkillLog, Warning, TEXT("MarkItemDirty called for Skill %s"), *Spec.Skill->GetName());
			ActivatableSkills.MarkItemDirty(Spec);
		}
		// AbilitySpecDirtiedCallbacks.Broadcast(Spec);
	}
	else
	{
		// Clients predicting should call MarkArrayDirty to force the internal replication map to be rebuilt.
		//DS_NETLOG(DSSkillLog, Warning, TEXT("MarkArrayDirty called on Client"));
		ActivatableSkills.MarkArrayDirty();
	}
}

bool UDSSkillControlComponent::IsOwnerActorAuthoritative() const
{
	return !bCachedIsNetSimulated;
}



/**
 * Attempts to activate the ability.
 *	-This function calls CanActivateAbility
 *	-This function handles instancing
 *	-This function handles networking and prediction
 *	-If all goes well, CallActivateAbility is called next.
 */
bool UDSSkillControlComponent::InternalTryActivateSkill(FDSSkillSpecHandle Handle, UDSSkillBase** OutInstancedAbility)
{
	// const FGameplayTag& NetworkFailTag = UAbilitySystemGlobals::Get().ActivateFailNetworkingTag;

// InternalTryActivateAbilityFailureTags.Reset();

	if (Handle.IsValid() == false)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalTryActivateSkill called with invalid Handle! SCC: %s. AvatarActor: %s"), *GetPathName(), *GetNameSafe(GetAvatarActor_Direct()));
		return false;
	}

	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalTryActivateSkill called with valid Handle but no matching ability was found. Handle: %s SCC: %s. AvatarActor: %s"), *Handle.ToString(), *GetPathName(), *GetNameSafe(GetAvatarActor_Direct()));
		return false;
	}

	const FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();

	// make sure the ActorInfo and then Actor on that FGameplayAbilityActorInfo are valid, if not bail out.
	if (ActorInfo == nullptr || !ActorInfo->SkillOwner.IsValid() || !ActorInfo->SkillAvatar.IsValid())
	{
		return false;
	}

	// This should only come from button presses/local instigation (AI, etc)
	ENetRole NetMode = ROLE_SimulatedProxy;

	// Use PC netmode if its there
	if (APlayerController* PC = ActorInfo->PlayerController.Get())
	{
		NetMode = PC->GetLocalRole();
	}
	// Fallback to avataractor otherwise. Edge case: avatar "dies" and becomes torn off and ROLE_Authority. We don't want to use this case (use PC role instead).
	else if (AActor* LocalAvatarActor = GetAvatarActor_Direct())
	{
		NetMode = LocalAvatarActor->GetLocalRole();
	}

	if (NetMode == ROLE_SimulatedProxy)
	{
		return false;
	}

	bool bIsLocal = SkillActorInfo->IsLocallyControlled();

	UDSSkillBase* Skill = Spec->Skill;

	if (!Skill)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalTryActivateSkill called with invalid Skill"));
		return false;
	}

	// Check to see if this a local only or server only ability, if so don't execute
	if (!bIsLocal)
	{
		if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalOnly || (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted))// && !InPredictionKey.IsValidKey() ) )
		{
			//DS_NETLOG(DSSkillLog, Warning, TEXT("Can't activate LocalOnly or LocalPredicted ability %s when not local! Net Execution Policy is %d."), *Skill->GetName(), (int32)Skill->GetNetExecutionPolicy());

			// if ( NetworkFailTag.IsValid() )
			// {
			// 	InternalTryActivateAbilityFailureTags.AddTag(NetworkFailTag);
			// NotifySkillFailed(Handle, Skill);
			// }

			return false;
		}
	}

	if (NetMode != ROLE_Authority && (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated))
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("Can't activate ServerOnly or ServerInitiated ability %s when not the server! Net Execution Policy is %d."), *Skill->GetName(), (int32)Skill->GetNetExecutionPolicy());

		// if ( NetworkFailTag.IsValid() )
		// {
		// 	InternalTryActivateAbilityFailureTags.AddTag(NetworkFailTag);
		// NotifySkillFailed(Handle, Skill);
		// }

		return false;
	}

	// If it's an instanced one, the instanced ability will be set, otherwise it will be null
	UDSSkillBase* InstancedSkill = Spec->GetPrimaryInstance();
	UDSSkillBase* SkillSource = InstancedSkill ? InstancedSkill : Skill;

	// if (TriggerEventData)
	// {
	// 	if (!SkillSource->ShouldSkillRespondToEvent(ActorInfo, TriggerEventData))
	// 	{
	// 		UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: Can't activate %s because ShouldAbilityRespondToEvent was false."), *GetNameSafe(GetOwner()), *Ability->GetName());
	// 		UE_VLOG(GetOwner(), VLogAbilitySystem, Verbose, TEXT("Can't activate %s because ShouldAbilityRespondToEvent was false."), *Ability->GetName());
	// 
	// 		NotifyAbilityFailed(Handle, AbilitySource, InternalTryActivateAbilityFailureTags);
	// 		return false;
	// 	}
	// }

	{
		// const FGameplayTagContainer* SourceTags = TriggerEventData ? &TriggerEventData->InstigatorTags : nullptr;
		// const FGameplayTagContainer* TargetTags = TriggerEventData ? &TriggerEventData->TargetTags : nullptr;
		// 
		// // If we have an instanced ability, call CanActivateAbility on it.
		// // Otherwise we always do a non instanced CanActivateAbility check using the CDO of the Ability.
		// FScopedCanActivateAbilityLogEnabler LogEnabler;
		// 
		// if ( !CanActivateAbilitySource->CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, &InternalTryActivateAbilityFailureTags) )
		// {
		// NotifySkillFailed(Handle, SkillSource);
		 	// return false;
		// }
	}

	// If we're instance per actor and we're already active, don't let us activate again as this breaks the graph
	if ( Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor )
	{
		if ( Spec->IsActive() )
		{
			if ( Skill->bRetriggerInstancedSkill && InstancedSkill )
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("%s: Ending %s prematurely to retrigger."), *GetNameSafe(GetOwner()), *Skill->GetName());

				bool bReplicateEndAbility = true;
				bool bWasCancelled = false;
				InstancedSkill->EndSkill(Handle, ActorInfo, bReplicateEndAbility, bWasCancelled);
			}
			else
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("Can't activate instanced per actor ability %s when their is already a currently active instance for this actor."), *Skill->GetName());
				return false;
			}
		}
	}

	// Make sure we have a primary
	if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor && !InstancedSkill)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalTryActivateSkill called but instanced ability is missing! NetMode: %d. Skill: %s"), (int32)NetMode, *Skill->GetName());
		return false;
	}

	// Setup a fresh ActivationInfo for this AbilitySpec.
	// Spec->ActivationInfo = FGameplayAbilityActivationInfo(ActorInfo->OwnerActor.Get());
	// FGameplayAbilityActivationInfo& ActivationInfo = Spec->ActivationInfo;

	// If we are the server or this is local only
	if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalOnly || (NetMode == ROLE_Authority))
	{
		// if we're the server and don't have a valid key or this ability should be started on the server create a new activation key
		bool bCreateNewServerKey = NetMode == ROLE_Authority &&
			( /*	!InPredictionKey.IsValidKey() ||*/
				(Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated ||
					Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly));
		// if ( bCreateNewServerKey )
		// {
		// 	ActivationInfo.ServerSetActivationPredictionKey(FPredictionKey::CreateNewServerInitiatedKey(this));
		// }
		// else if ( InPredictionKey.IsValidKey() )
		// {
		// 	// Otherwise if available, set the prediction key to what was passed up
		// 	ActivationInfo.ServerSetActivationPredictionKey(InPredictionKey);
		// }
		
		// we may have changed the prediction key so we need to update the scoped key to match
		// FScopedPredictionWindow ScopedPredictionWindow(this, ActivationInfo.GetActivationPredictionKey());

		// ----------------------------------------------
		// Tell the client that you activated it (if we're not local and not server only)
		// ----------------------------------------------
		if (!bIsLocal && Skill->GetNetExecutionPolicy() != ESkillNetExecutionPolicy::ServerOnly)
		{
			// if ( TriggerEventData )
			// {
			// 	ClientActivateAbilitySucceedWithEventData(Handle, ActivationInfo.GetActivationPredictionKey(), *TriggerEventData);
			// }
			// else
			// {
			ClientActivateSkillSucceed(Handle);
			// }

			// This will get copied into the instanced abilities
			// ActivationInfo.bCanBeEndedByOtherInstance = Ability->bServerRespectsRemoteAbilityCancellation;
		}

		// ----------------------------------------------
		//	Call ActivateAbility (note this could end the ability too!)
		// ----------------------------------------------

		// Create instance of this ability if necessary
		if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
		{
			InstancedSkill = CreateNewInstanceOfSkill(*Spec, Skill);
			InstancedSkill->CallActivateSkill(Handle, ActorInfo);
		}
		else if (InstancedSkill)
		{
			InstancedSkill->CallActivateSkill(Handle, ActorInfo);
		}
		else
		{
			Skill->CallActivateSkill(Handle, ActorInfo);
		}
	}
	else if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted)
	{
		// Flush server moves that occurred before this ability activation so that the server receives the RPCs in the correct order
		// Necessary to prevent abilities that trigger animation root motion or impact movement from causing network corrections
		if (!ActorInfo->IsNetAuthority())
		{
			ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->SkillAvatar.Get());
			if (AvatarCharacter)
			{
				UCharacterMovementComponent* AvatarCharMoveComp = Cast<UCharacterMovementComponent>(AvatarCharacter->GetMovementComponent());
				if (AvatarCharMoveComp)
				{
					AvatarCharMoveComp->FlushServerMoves();
				}
			}
		}

		// This execution is now officially EGameplayAbilityActivationMode:Predicting and has a PredictionKey
		// FScopedPredictionWindow ScopedPredictionWindow(this, true);
		// 
		// ActivationInfo.SetPredicting(ScopedPredictionKey);
		// 
		// // This must be called immediately after GeneratePredictionKey to prevent problems with recursively activating abilities
		// if ( TriggerEventData )
		// {
		// 	ServerTryActivateAbilityWithEventData(Handle, Spec->InputPressed, ScopedPredictionKey, *TriggerEventData);
		// }
		// else
		// {
			CallServerTryActivateSkill(Handle, Spec->InputPressed);
		// }

		

		// When this prediction key is caught up, we better know if the ability was confirmed or rejected
		// ScopedPredictionKey.NewCaughtUpDelegate().BindUObject(this, &UAbilitySystemComponent::OnClientActivateAbilityCaughtUp, Handle, ScopedPredictionKey.Current);

		if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
		{
			// For now, only NonReplicated + InstancedPerExecution abilities can be Predictive.
			// We lack the code to predict spawning an instance of the execution and then merge/combine
			// with the server spawned version when it arrives.

			if (Skill->GetReplicationPolicy() == ESkillReplicationPolicy::ReplicateNo)
			{
				InstancedSkill = CreateNewInstanceOfSkill(*Spec, Skill);
				InstancedSkill->CallActivateSkill(Handle, ActorInfo);
			}
			else
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalTryActivateSkill called on ability %s that is InstancePerExecution and Replicated. This is an invalid configuration."), *Skill->GetName());
			}
		}
		else if (InstancedSkill)
		{
			 InstancedSkill->CallActivateSkill(Handle, ActorInfo);
		}
		else
		{
			Skill ->CallActivateSkill(Handle, ActorInfo);
		}
	}

	if (InstancedSkill)
	{
		if (OutInstancedAbility)
		{
			*OutInstancedAbility = InstancedSkill;
		}

		// UGameplayAbility::PreActivate actually sets this internally (via SetCurrentInfo) which happens after replication (this is only set locally).  Let's cautiously remove this code.
		// if ( CVarAbilitySystemSetActivationInfoMultipleTimes.GetValueOnGameThread() )
		// {
		// 	InstancedSkill->SetCurrentActivationInfo(ActivationInfo);	// Need to push this to the ability if it was instanced.
		// }
	}

	MarkSkillSpecDirty(*Spec);

	//DS_NETLOG(DSSkillLog, Warning, TEXT("%s: Activated [%s] %s."), *GetNameSafe(GetOwner()), *Spec->Handle.ToString(), *GetNameSafe(SkillSource));
	// AbilityLastActivatedTime = GetWorld()->GetTimeSeconds();

	return true;
}

void UDSSkillControlComponent::OnRep_SkillOwnerActor()
{
}

void UDSSkillControlComponent::OnRep_SkillAvatarActor()
{
}

bool UDSSkillControlComponent::IsSkillInputBlocked(int32 InputID) const
{
	// Check if this ability's input binding is currently blocked
	const TArray<uint8>& ConstBlockedSkillBindings = GetBlockedSkillBindings();
	if (InputID >= 0 && InputID < ConstBlockedSkillBindings.Num() && ConstBlockedSkillBindings[InputID] > 0)
	{
		return true;
	}

	return false;
}

const TArray<uint8>& UDSSkillControlComponent::GetBlockedSkillBindings() const
{
	return BlockedSkillBindings;
}




void UDSSkillControlComponent::OnRep_ActivatableSkills()
{
	//DS_NETLOG(DSSkillLog, Warning, TEXT("OnRep_ActivatableSkills"));

	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		const UDSSkillBase* SpecSkill = Spec.Skill;
		if (!SpecSkill)
		{
			// Queue up another call to make sure this gets run again, as our abilities haven't replicated yet
			GetWorld()->GetTimerManager().SetTimer(OnRep_ActivateSkillsTimerHandle, this, &UDSSkillControlComponent::OnRep_ActivatableSkills, 0.5);
			return;
		}
	}

	CheckForClearedSkills();

	// Try to run any pending activations that couldn't run before. If they don't work now, kill them

	for (const FPendingSkillInfo& PendingAbilityInfo : PendingServerActivatedSkills)
	{
		if (PendingAbilityInfo.bPartiallyActivated)
		{
			// ClientActivateAbilitySucceedWithEventData_Implementation(PendingAbilityInfo.Handle, PendingAbilityInfo.PredictionKey, PendingAbilityInfo.TriggerEventData);
		}
		else
		{
			ClientTryActivateSkill(PendingAbilityInfo.Handle);
		}
	}
	PendingServerActivatedSkills.Empty();
}

void UDSSkillControlComponent::OnAddSkill(FDSSkillSpec& Spec)
{
	if (!Spec.Skill)
	{
		return;
	}

	const UDSSkillBase* SpecSkill = Spec.Skill;
	if (SpecSkill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor && SpecSkill->GetReplicationPolicy() == ESkillReplicationPolicy::ReplicateNo)
	{
		// 만약 Spec이 NonReplicatedInstances이고, 배열이 비어 있다면 CreateNewInstanceOfSkill()을 호출하여 새로운 스킬 인스턴스를 생성한다.
		if (Spec.NonReplicatedInstances.Num() == 0)
		{
			CreateNewInstanceOfSkill(Spec, SpecSkill);
		}
	}

	// 이미 SkillSpec 대해 생성된 “Primary Instance”가 있다면, 그 인스턴스의 OnAddSkill를 호출한다.
	UDSSkillBase* PrimaryInstance = Spec.GetPrimaryInstance();
	if (PrimaryInstance)
	{
		PrimaryInstance->OnAddSkill(SkillActorInfo.Get(), Spec);
	}
	else
	{
		Spec.Skill->OnAddSkill(SkillActorInfo.Get(), Spec);
	}


	if(SkillActorInfo.Get())
	{
		//DS_NETLOG(DSSkillLog, Log, TEXT("SkillOwner : %s, SkillAvatar : %s"), *SkillActorInfo->SkillOwner->GetName(), *SkillActorInfo->SkillAvatar->GetName());
	}
	else
	{
		//DS_NETLOG(DSSkillLog, Log, TEXT("No ActorInfo"));
	}
}

void UDSSkillControlComponent::OnRemoveSkill(FDSSkillSpec& Spec)
{
	if (!Spec.Skill)
	{
		return;
	}

	//DS_NETLOG(DSSkillLog, Log, TEXT("%s: Removing Ability [%s] %s"), *GetNameSafe(GetOwner()), *Spec.Handle.ToString(), *GetNameSafe(Spec.Skill));

	TArray<UDSSkillBase*> Instances = Spec.GetSkillInstances();

	for (auto Instance : Instances)
	{
		if (Instance)
		{
			if (Instance->IsActive())
			{
				// End the ability but don't replicate it, OnRemoveAbility gets replicated
				bool bReplicateEndAbility = false;
				bool bWasCancelled = false;
				Instance->EndSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, bReplicateEndAbility, bWasCancelled);
			}
			else
			{
				// Ability isn't active, but still needs to be destroyed
				if (GetOwnerRole() == ROLE_Authority)
				{
					// Only destroy if we're the server or this isn't replicated. Can't destroy on the client or replication will fail when it replicates the end state
					RemoveReplicatedInstancedSkill(Instance);
				}

				if (Instance->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
				{
					//DS_NETLOG(DSSkillLog, Log, TEXT("%s was InActive, yet still instanced during OnRemove"), *Instance->GetName());
					Instance->MarkAsGarbage();
				}
			}
		}
	}

	// Notify the ability that it has been removed.  It follows the same pattern as OnGiveAbility() and is only called on the primary instance of the ability or the CDO.
	UDSSkillBase* PrimaryInstance = Spec.GetPrimaryInstance();
	if (PrimaryInstance)
	{
		PrimaryInstance->OnRemoveSkill(SkillActorInfo.Get(), Spec);

		// Make sure we remove this before marking it as garbage.
		if (GetOwnerRole() == ROLE_Authority)
		{
			RemoveReplicatedInstancedSkill(PrimaryInstance);
		}
		PrimaryInstance->MarkAsGarbage();
	}
	else
	{
		// If we're non-instanced and still active, we need to End
		if (Spec.IsActive())
		{
			if (ensureMsgf(Spec.Skill->GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced, TEXT("We should never have an instanced Gameplay Ability that is still active by this point. All instances should have EndAbility called just before here.")))
			{
				// Seems like it should be cancelled, but we're just following the existing pattern (could be due to functionality from OnRep)
				constexpr bool bReplicateEndAbility = false;
				constexpr bool bWasCancelled = false;
				Spec.Skill->EndSkill(Spec.Handle,SkillActorInfo.Get(), bReplicateEndAbility, bWasCancelled);
			}
		}

		Spec.Skill->OnRemoveSkill(SkillActorInfo.Get(), Spec);
	}

	Spec.ReplicatedInstances.Empty();
	Spec.NonReplicatedInstances.Empty();
}

FDSSkillSpecHandle UDSSkillControlComponent::AddSkill(const FDSSkillSpec& Spec)
{
	//DS_NETLOG(DSSkillLog, Log, TEXT("AddSkill"));

	if (!IsValid(Spec.Skill))
	{
		// ABILITY_LOG(Error, TEXT("GiveAbility called with an invalid Ability Class."));
		//DS_NETLOG(DSSkillLog, Warning, TEXT("AttachSkill called with an invalid Skill Class."));
	}
	if (!IsOwnerActorAuthoritative())
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("AttachSkill called on Skill %s on the client, not allowed!"), *Spec.Skill->GetName());
		// ABILITY_LOG(Error, TEXT("GiveAbility called on ability %s on the client, not allowed!"), *Spec.Ability->GetName());
	}

	// If locked, add to pending list. The Spec.Handle is not regenerated when we receive, so returning this is ok.
	// if ( AbilityScopeLockCount > 0 )
	// {
	// 	AbilityPendingAdds.Add(Spec);
	// 	return Spec.Handle;
	// }

	// ABILITYLIST_SCOPE_LOCK();

	FDSSkillSpec& OwnedSpec = ActivatableSkills.Items[ActivatableSkills.Items.Add(Spec)];

	if (OwnedSpec.Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor)
	{
		CreateNewInstanceOfSkill(OwnedSpec, Spec.Skill);
	}

	OnAddSkill(OwnedSpec);
	MarkSkillSpecDirty(OwnedSpec, true);

	return OwnedSpec.Handle;
}

bool UDSSkillControlComponent::TryActivateSkill(FDSSkillSpecHandle SkillToActivate, bool bAllowRemoteActivation)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(SkillToActivate);
	if (!Spec)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("TryActivateSkill called with invalid Handle"));
		return false;
	}

	// don't activate abilities that are waiting to be removed
	if (Spec->PendingRemove || Spec->RemoveAfterActivation)
	{
		return false;
	}

	UDSSkillBase* Skill= Spec->Skill;

	if (!IsValid(Skill))
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("TryActivateSkill called with invalid Skill"));
		return false;
	}

	const FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();

	// make sure the ActorInfo and then Actor on that FGameplayAbilityActorInfo are valid, if not bail out.
	if (ActorInfo == nullptr || !ActorInfo->SkillOwner.IsValid() || !ActorInfo->SkillAvatar.IsValid())
	{
		return false;
	}


	const ENetRole NetMode = ActorInfo->SkillAvatar->GetLocalRole();

	// This should only come from button presses/local instigation (AI, etc).
	if (NetMode == ROLE_SimulatedProxy)
	{
		return false;
	}

	bool bIsLocal = SkillActorInfo->IsLocallyControlled();

	// Check to see if this a local only or server only ability, if so either remotely execute or fail
	if (!bIsLocal && (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalOnly || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted))
	{
		if (bAllowRemoteActivation)
		{
			ClientTryActivateSkill(SkillToActivate);
			return true;
		}

		//DS_NETLOG(DSSkillLog, Warning, TEXT("Can't activate LocalOnly or LocalPredicted ability %s when not local."), *Skill->GetName());
		return false;
	}

	if (NetMode != ROLE_Authority && (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated))
	{
		if (bAllowRemoteActivation)
		{
			// FScopedCanActivateAbilityLogEnabler LogEnabler;
			if (Skill->CanActivateSkill(SkillToActivate, ActorInfo)) //, nullptr, nullptr, &FailureTags))
			{
				// No prediction key, server will assign a server-generated key
				CallServerTryActivateSkill(SkillToActivate, Spec->InputPressed); //, FPredictionKey());
				return true;
			}
			else
			{
				NotifySkillFailed(SkillToActivate, Skill);
				return false;
			}
		}

		//DS_NETLOG(DSSkillLog, Warning, TEXT("Can't activate ServerOnly or ServerInitiated ability %s when not the server."), *Skill->GetName());
		return false;
	}
	//DS_NETLOG(DSSkillLog, Warning, TEXT(""));
	return InternalTryActivateSkill(SkillToActivate);
}



void UDSSkillControlComponent::CallServerTryActivateSkill(FDSSkillSpecHandle SkillToActivate, bool InputPressed)
{
	//DS_NETLOG(DSSkillLog, Warning, TEXT("::CallServerTryActivateAbility %s "), *SkillToActivate.ToString());

	// //DS_NETLOG(DSSkillLog, Warning, TEXT("    NO BATCH IN SCOPE"));
	ServerTryActivateSkill(SkillToActivate, InputPressed);
}

void UDSSkillControlComponent::CallServerEndSkill(FDSSkillSpecHandle SkillHandle)
{
	//DS_NETLOG(DSSkillLog, Warning, TEXT("::CallServerEndAbility %s "), *SkillHandle.ToString());

	ServerEndSkill(SkillHandle);
}

void UDSSkillControlComponent::ServerEndSkill_Implementation(FDSSkillSpecHandle SkillToEnd)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(SkillToEnd);

	if (Spec && Spec->Skill &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
	{
		RemoteEndOrCancelSkill(SkillToEnd, false);
	}
}

bool UDSSkillControlComponent::ServerEndSkill_Validate(FDSSkillSpecHandle Handle)
{
	return true;
}
void UDSSkillControlComponent::ServerTryActivateSkill_Implementation(FDSSkillSpecHandle Handle, bool InputPressed)
{
	InternalServerTryActivateSkill(Handle, InputPressed);
}
bool UDSSkillControlComponent::ServerTryActivateSkill_Validate(FDSSkillSpecHandle SkillToActivate, bool InputPressed)
{
	return true;
}

void UDSSkillControlComponent::ClientTryActivateSkill_Implementation(FDSSkillSpecHandle Handle)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	//DS_NETLOG(DSSkillLog, Warning, TEXT(""));
	if (!Spec)
	{
		// Can happen if the client gets told to activate an ability the same frame that abilities are added on the server
		FPendingSkillInfo AbilityInfo;
		AbilityInfo.Handle = Handle;
		AbilityInfo.bPartiallyActivated = false;

		// This won't add it if we're currently being called from the pending list
		PendingServerActivatedSkills.AddUnique(AbilityInfo);
		return;
	}
	InternalTryActivateSkill(Handle);
}

void UDSSkillControlComponent::NotifySkillCommit(UDSSkillBase* Skill)
{
	// AbilityCommittedCallbacks.Broadcast(Skill);
}

void UDSSkillControlComponent::NotifySkillActivated(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill)
{
	// AbilityActivatedCallbacks.Broadcast(Skill);
}

void UDSSkillControlComponent::NotifySkillFailed(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill)
{
	//DS_NETLOG(DSSkillLog, Warning, TEXT("NotifySkillFailed : %s Skill"), *Skill->GetName());
	// AbilityFailedCallbacks.Broadcast(Skill);
}


void UDSSkillControlComponent::NotifySkillEnded(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCancelled)
{
	check(Skill);
	
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);

	if (Spec == nullptr)
	{
		// The ability spec may have been removed while we were ending. We can assume everything was cleaned up if the spec isnt here.
		return;
	}

	// UE_LOG(LogAbilitySystem, Log, TEXT("%s: Ended [%s] %s. Level: %d. WasCancelled: %d."), *GetNameSafe(GetOwner()), *Handle.ToString(), Spec->GetPrimaryInstance() ? *Spec->GetPrimaryInstance()->GetName() : *Ability->GetName(), Spec->Level, bWasCancelled);
	// UE_VLOG(GetOwner(), VLogAbilitySystem, Log, TEXT("Ended [%s] %s. Level: %d. WasCancelled: %d."), *Handle.ToString(), Spec->GetPrimaryInstance() ? *Spec->GetPrimaryInstance()->GetName() : *Ability->GetName(), Spec->Level, bWasCancelled);

	ENetRole OwnerRole = GetOwnerRole();

	// If AnimatingAbility ended, clear the pointer
	// if (LocalAnimMontageInfo.AnimatingAbility.Get() == Skill)
	// {
	// 	ClearAnimatingAbility(Ability);
	// }

	// check to make sure we do not cause a roll over to uint8 by decrementing when it is 0
	// if (ensureMsgf(Spec->ActiveCount > 0, TEXT("NotifyAbilityEnded called when the Spec->ActiveCount <= 0 for ability %s"), *Ability->GetName()))
	{
		Spec->ActiveCount--;
	}

	// Broadcast that the ability ended
	// SkillEndedCallbacks.Broadcast(Ability);
	
	// OnSkillEnded.Broadcast(FSkillEndedData(Skill, Handle, false, bWasCancelled));

	// Above callbacks could have invalidated the Spec pointer, so find it again
	Spec = FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		// ABILITY_LOG(Error, TEXT("%hs(%s): %s lost its active handle halfway through the function."), __func__, *GetNameSafe(Ability), *Handle.ToString());
		return;
	}

	/** If this is instanced per execution or flagged for cleanup, mark pending kill and remove it from our instanced lists if we are the authority */
	if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
	{
		check(Skill->HasAnyFlags(RF_ClassDefaultObject) == false);	// Should never be calling this on a CDO for an instanced ability!

		if (Skill->GetReplicationPolicy() != ESkillReplicationPolicy::ReplicateNo)
		{
			if (OwnerRole == ROLE_Authority)
			{
				Spec->ReplicatedInstances.Remove(Skill);
				RemoveReplicatedInstancedSkill(Skill);
			}
		}
		else
		{
			Spec->NonReplicatedInstances.Remove(Skill);
		}

		Skill->MarkAsGarbage();
	}

	if (OwnerRole == ROLE_Authority)
	{
		if (Spec->RemoveAfterActivation && !Spec->IsActive())
		{
			// If we should remove after activation and there are no more active instances, kill it now
			ClearSkill(Handle);
		}
		else
		{
			MarkSkillSpecDirty(*Spec);
		}
	}
}

void UDSSkillControlComponent::ReplicateEndOrCancelSkill(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCanceled)
{
	if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated)
	{
		// Only replicate ending if policy is predictive
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (!SkillActorInfo->IsLocallyControlled())
			{
				// Only tell the client about the end/cancel ability if we're not the local controller
				if (bWasCanceled)
				{
					ClientCancelSkill(Handle); 
				}
				else
				{
					ClientEndSkill(Handle);
				}
			}
		}
		else if (Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination && Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
		{
			// This passes up the current prediction key if we have one
			if (bWasCanceled)
			{
				ServerCancelSkill(Handle); 
			}
			else
			{
				CallServerEndSkill(Handle);
			}
		}
	}
}

void UDSSkillControlComponent::RemoveReplicatedInstancedSkill(UDSSkillBase* Skill)
{
	const bool bWasRemoved = GetReplicatedInstancedSkills_Mutable().RemoveSingle(Skill) > 0;
	if (bWasRemoved && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		RemoveReplicatedSubObject(Skill);
	}
}

void UDSSkillControlComponent::RemoteEndOrCancelSkill(FDSSkillSpecHandle SkillToEnd, bool bWasCanceled)
{
	FDSSkillSpec* SkillSpec = FindSkillSpecFromHandle(SkillToEnd);
	if (SkillSpec && SkillSpec->Skill && SkillSpec->IsActive())
	{
		// Handle non-instanced case, which cannot perform prediction key validation
		if (SkillSpec->Skill->GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced)
		{
			// End/Cancel the ability but don't replicate it back to whoever called us
			if (bWasCanceled)
			{
				SkillSpec->Skill->CancelSkill(SkillToEnd, SkillActorInfo.Get(), false);
			}
			else
			{
				SkillSpec->Skill->EndSkill(SkillToEnd, SkillActorInfo.Get(), false, bWasCanceled);
			}
		}
		// else
		// {
		// 	TArray<UDSSkillBase*> Instances = SkillSpec->GetSkillInstances();
		// 
		// 	for (auto Instance : Instances)
		// 	{
		// 		UE_CLOG(Instance == nullptr, LogAbilitySystem, Fatal, TEXT("UAbilitySystemComponent::RemoteEndOrCancelAbility null instance for %s"), *GetNameSafe(SkillSpec->Ability));
		// 
		// 		// Check if the ability is the same prediction key (can both by 0) and has been confirmed. If so cancel it.
		// 		if (Instance->GetCurrentActivationInfoRef().GetActivationPredictionKey() == ActivationInfo.GetActivationPredictionKey())
		// 		{
		// 			// Let the ability know that the remote instance has ended, even if we aren't about to end it here.
		// 			Instance->SetRemoteInstanceHasEnded();
		// 
		// 			if (Instance->GetCurrentActivationInfoRef().bCanBeEndedByOtherInstance)
		// 			{
		// 				// End/Cancel the ability but don't replicate it back to whoever called us
		// 				if (bWasCanceled)
		// 				{
		// 					ForceCancelSkillDueToReplication(Instance);
		// 				}
		// 				else
		// 				{
		// 					Instance->EndSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, false, bWasCanceled);
		// 				}
		// 			}
		// 		}
		// 	}
		// }
	}
}

void UDSSkillControlComponent::InternalServerTryActivateSkill(FDSSkillSpecHandle Handle, bool InputPressed)
{
#if WITH_SERVER_CODE
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// if (DenyClientActivation > 0)
	// {
	// 	DenyClientActivation--;
	// 	ClientActivateSkillFailed(Handle, PredictionKey.Current);
	// 	return;
	// }
#endif

	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		// Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of skill with invalid SpecHandle!"));
		ClientActivateSkillFailed(Handle);
		return;
	}

	const UDSSkillBase* SkillToActivate = Spec->Skill;

	if (!ensure(SkillToActivate))
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of unconfigured spec skill!"));
		ClientActivateSkillFailed(Handle);
		return;
	}

	// Ignore a client trying to activate an ability requiring server execution
	if (SkillToActivate->GetNetSecurityPolicy() == ESkillNetSecurityPolicy::ServerOnlyExecution ||
		SkillToActivate->GetNetSecurityPolicy() == ESkillNetSecurityPolicy::ServerOnly)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of %s due to security policy violation."), *GetNameSafe(SkillToActivate));
		ClientActivateSkillFailed(Handle);
		return;
	}
	ensure(SkillActorInfo.IsValid());

	SCOPE_CYCLE_UOBJECT(Skill, SkillToActivate);

	UDSSkillBase* InstancedSkill = nullptr;
	Spec->InputPressed = true;

	// Attempt to activate the ability (server side) and tell the client if it succeeded or failed.
	if (InternalTryActivateSkill(Handle,  &InstancedSkill))
	{
		// TryActivateAbility handles notifying the client of success
	}
	else
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of %s. "), *GetNameSafe(Spec->Skill));
		ClientActivateSkillFailed(Handle);
		Spec->InputPressed = false;

		MarkSkillSpecDirty(*Spec);
	}
#endif
}

void UDSSkillControlComponent::ClientActivateSkillFailed_Implementation(FDSSkillSpecHandle Handle)
{
	// Find the actual UGameplayAbility		
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (Spec == nullptr)
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("ClientActivateSkillFailed_Implementation.Skill: Could not find!"));
		return;
	}

	//DS_NETLOG(DSSkillLog, Warning,TEXT("ClientActivateSkillFailed_Implementation. Ability: %s"), *GetNameSafe(Spec->Skill));

	TArray<UDSSkillBase*> Instances = Spec->GetSkillInstances();
	for (UDSSkillBase* Ability : Instances)
	{
		Ability->EndSkill(Handle, SkillActorInfo.Get(),true, false);
	}
}

void UDSSkillControlComponent::ClearSkill(const FDSSkillSpecHandle& Handle)
{
	if (!IsOwnerActorAuthoritative())
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("Attempted to call ClearSkill() on the client. This is not allowed!"));
		return;
	}

	for (int Idx = 0; Idx < ActivatableSkills.Items.Num(); ++Idx)
	{
		check(ActivatableSkills.Items[Idx].Handle.IsValid());
		if (ActivatableSkills.Items[Idx].Handle == Handle)
		{
			OnRemoveSkill(ActivatableSkills.Items[Idx]);
			ActivatableSkills.Items.RemoveAtSwap(Idx);
			ActivatableSkills.MarkArrayDirty();
			CheckForClearedSkills();
		}
	}

	//DS_NETLOG(DSSkillLog, Log, TEXT("ClearSkill"));
}

void UDSSkillControlComponent::ClientCancelSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	RemoteEndOrCancelSkill(SkillToCancel, true);
}

bool UDSSkillControlComponent::ServerCancelSkill_Validate(struct FDSSkillSpecHandle SkillToCancel)
{
	return true;
}

void UDSSkillControlComponent::ServerCancelSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(SkillToCancel);

	if (Spec && Spec->Skill &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
	{
		RemoteEndOrCancelSkill(SkillToCancel, true);
	}
}

void UDSSkillControlComponent::ClientEndSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	RemoteEndOrCancelSkill(SkillToCancel, false);
}


void UDSSkillControlComponent::ClientActivateSkillSucceed_Implementation(FDSSkillSpecHandle Handle)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		// Can happen if the client gets told to activate an ability the same frame that abilities are added on the server
		FPendingSkillInfo SkillInfo;
		// SkillInfo.PredictionKey = PredictionKey;
		SkillInfo.Handle = Handle;
		// SkillInfo.TriggerEventData = TriggerEventData;
		SkillInfo.bPartiallyActivated = true;

		// This won't add it if we're currently being called from the pending list
		PendingServerActivatedSkills.AddUnique(SkillInfo);
		return;
	}

	UDSSkillBase* SkillToActivate = Spec->Skill;

	check(SkillToActivate);
	ensure(SkillActorInfo.IsValid());

	// Spec->ActivationInfo.SetActivationConfirmed();

	//DS_NETLOG(DSSkillLog, Warning, TEXT("%s: Server Confirmed [%s] %s"), *GetNameSafe(GetOwner()), *Handle.ToString(), *GetNameSafe(SkillToActivate));

	// Fixme: We need a better way to link up/reconcile predictive replicated abilities. It would be ideal if we could predictively spawn an
	// ability and then replace/link it with the server spawned one once the server has confirmed it.

	// 클라이언트에서 먼저 실행하고, 서버에서 최종으로 결정하는 경우
	if (SkillToActivate->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted)
	{
		// 하나의 CDO 공유하는 경우
		if (SkillToActivate->GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced)
		{	
			//
			SkillToActivate->ConfirmActivateSucceed(); // This doesn't do anything for non instanced
		}
		// 상태 저장이 가능한 경우
		else
		{
			bool found = false;
			TArray<UDSSkillBase*> Instances = Spec->GetSkillInstances();
			for (UDSSkillBase* LocalAbility : Instances)
			{
				if (LocalAbility != nullptr ) 
				{
					LocalAbility->ConfirmActivateSucceed();
					found = true;
					break;
				}
			}

			if (!found)
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("Ability %s was confirmed by server but no longer exists on client "), *SkillToActivate->GetName());
			}
		}
	}
	// 서버에서 먼저 실행, 서버에서만 실행, 클라에서만 실행되는 경우
	else
	{
		// 실행할 때마다 생성되는 경우
		if (SkillToActivate->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
		{
			// Need to instantiate this in order to execute
			UDSSkillBase* InstancedSkill = CreateNewInstanceOfSkill(*Spec, SkillToActivate);
			InstancedSkill->CallActivateSkill(Handle,SkillActorInfo.Get());
		}
		// 하나의 CDO를 공유하지 않는 경우, 상태 저장이 되는 경우
		else if (SkillToActivate->GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
		{
			UDSSkillBase* InstancedAbility = Spec->GetPrimaryInstance();

			if (!InstancedAbility)
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("Skill %s cannot be activated on the client because it's missing a primary instance! "), *SkillToActivate->GetName());
				return;
			}
			InstancedAbility->CallActivateSkill(Handle, SkillActorInfo.Get());
		}
		// 하나의 CDO를 공유하는 경우
		else
		{
			SkillToActivate->CallActivateSkill(Handle, SkillActorInfo.Get());
		}
	}
}


// ---------------------------------------------------------- ActorInfo ----------------------------------------------------------

void UDSSkillControlComponent::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	check(SkillActorInfo.IsValid());

	bool WasAbilityActorNull = (SkillActorInfo->SkillAvatar == nullptr);
	bool AvatarChanged = (InAvatarActor != SkillActorInfo->SkillAvatar);

	SkillActorInfo->InitFromActor(InOwnerActor, InAvatarActor, this);

	SetOwnerActor(InOwnerActor);

	const AActor* PrevAvatarActor = GetAvatarActor_Direct();
	SetAvatarActor_Direct(InAvatarActor);

	if (AvatarChanged)
	{
		for (FDSSkillSpec& Spec : ActivatableSkills.Items)
		{
			if (Spec.Skill)
			{
				if (Spec.Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor)
				{
					UDSSkillBase* SkillInstance = Spec.GetPrimaryInstance();
					if (SkillInstance)
					{
						SkillInstance->OnAvatarSet(SkillActorInfo.Get(), Spec);
					}
				}
				else
				{
					Spec.Skill->OnAvatarSet(SkillActorInfo.Get(), Spec);
				}
			}
		}
	}

	// LocalAnimMontageInfo = FGameplayAbilityLocalAnimMontage();
// if (IsOwnerActorAuthoritative())
// {
// 	SetRepAnimMontageInfo(FGameplayAbilityRepAnimMontage());
// }
// 
// if (bPendingMontageRep)
// {
// 	OnRep_ReplicatedAnimMontage();
// }
}

void UDSSkillControlComponent::RefreshSkillActorInfo()
{
	check(SkillActorInfo.IsValid());
	SkillActorInfo->InitFromActor(SkillActorInfo->SkillOwner.Get(), SkillActorInfo->SkillAvatar.Get(), this);
}

void UDSSkillControlComponent::SetOwnerActor(AActor* NewOwnerActor)
{
	if (SkillOwnerActor)
	{
		SkillOwnerActor->OnDestroyed.RemoveDynamic(this, &UDSSkillControlComponent::OnOwnerActorDestroyed);
	}
	SkillOwnerActor = NewOwnerActor;
	if (SkillOwnerActor)
	{
		SkillOwnerActor->OnDestroyed.AddUniqueDynamic(this, &UDSSkillControlComponent::OnOwnerActorDestroyed);
	}
}

void UDSSkillControlComponent::SetAvatarActor_Direct(AActor* NewAvatarActor)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(UDSSkillControlComponent, SkillAvatarActor, this);
	if (SkillAvatarActor)
	{
		SkillAvatarActor->OnDestroyed.RemoveDynamic(this, &UDSSkillControlComponent::OnAvatarActorDestroyed);
	}
	SkillAvatarActor = NewAvatarActor;
	if (SkillAvatarActor)
	{
		SkillAvatarActor->OnDestroyed.AddUniqueDynamic(this, &UDSSkillControlComponent::OnAvatarActorDestroyed);
	}
}

AActor* UDSSkillControlComponent::GetAvatarActor() const
{
	check(SkillActorInfo.IsValid());
	return SkillActorInfo->SkillAvatar.Get();
}

void UDSSkillControlComponent::SetAvatarActor(AActor* InAvatarActor)
{
	check(SkillActorInfo.IsValid());
	InitSkillActorInfo(GetOwnerActor(), InAvatarActor);
}

void UDSSkillControlComponent::OnAvatarActorDestroyed(AActor* InActor)
{
	if (InActor == SkillAvatarActor)
	{
		SkillAvatarActor = nullptr;
		MARK_PROPERTY_DIRTY_FROM_NAME(UDSSkillControlComponent, SkillAvatarActor, this);
	}
}

void UDSSkillControlComponent::OnOwnerActorDestroyed(AActor* InActor)
{
	if (InActor == SkillOwnerActor)
	{
		SkillOwnerActor = nullptr;
		MARK_PROPERTY_DIRTY_FROM_NAME(UDSSkillControlComponent, SkillOwnerActor, this);
	}
}


void UDSSkillControlComponent::ClearActorInfo()
{
	check(SkillActorInfo.IsValid());
	SkillActorInfo->ClearActorInfo();
	SetOwnerActor(nullptr);
	SetAvatarActor_Direct(nullptr);
}

void UDSSkillControlComponent::ClearAllSkills()
{
	if (!IsOwnerActorAuthoritative())
	{
		//DS_NETLOG(DSSkillLog, Warning, TEXT("Attempted to call ClearAllAbilities() without authority."));
		return;
	}

	// Note we aren't marking any old abilities pending kill. This shouldn't matter since they will be garbage collected.
	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		OnRemoveSkill(Spec);
	}

	ActivatableSkills.Items.Empty(ActivatableSkills.Items.Num());
	ActivatableSkills.MarkArrayDirty();

	CheckForClearedSkills();
	bSkillPendingClearAll = false;
}
void UDSSkillControlComponent::CheckForClearedSkills()
{
	TArray<TObjectPtr<UDSSkillBase>>& ReplicatedSkills = GetReplicatedInstancedSkills_Mutable();
	for (int32 i = 0; i < ReplicatedSkills.Num(); i++)
	{
		UDSSkillBase* Skill = ReplicatedSkills[i];

		if (!IsValid(Skill))
		{
			if (IsUsingRegisteredSubObjectList())
			{
				RemoveReplicatedSubObject(Skill);
			}

			ReplicatedSkills.RemoveAt(i);
			i--;
		}
	}
}


// ---------------------------------------------------------- Cancel Skill ----------------------------------------------------------

void UDSSkillControlComponent::CancelSkillSpec(FDSSkillSpec& Spec, UDSSkillBase* Ignore)
{
	FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();

	if (Spec.Skill->GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
	{
		// We need to cancel spawned instance, not the CDO
		TArray<UDSSkillBase*> SkillsToCancel = Spec.GetSkillInstances();
		for (UDSSkillBase* InstanceSkill : SkillsToCancel)
		{
			if (InstanceSkill && Ignore != InstanceSkill)
			{
				//DS_NETLOG(DSSkillLog, Warning, TEXT("CancelSkillSpec [%s]"), *InstanceSkill->GetName());
				InstanceSkill->CancelSkill(Spec.Handle, ActorInfo, true);
			}
		}
	}
	else
	{
		// Try to cancel the non instanced, this may not necessarily work
		//DS_NETLOG(DSSkillLog, Warning, TEXT("CancelSkillSpec [%s]"), *Spec.Skill->GetName());
		Spec.Skill->CancelSkill(Spec.Handle, ActorInfo, true);
	}
	MarkSkillSpecDirty(Spec);
}

void UDSSkillControlComponent::CancelSkill(UDSSkillBase* Skill)
{
	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.Skill == Skill)
		{
			CancelSkillSpec(Spec, nullptr);
		}
	}
}

void UDSSkillControlComponent::CancelSkillHandle(const FDSSkillSpecHandle& SkillHandle)
{
	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.Handle == SkillHandle)
		{
			CancelSkillSpec(Spec, nullptr);
			return;
		}
	}
}

void UDSSkillControlComponent::CancelAllSkills(UDSSkillBase* Ignore)
{
	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.IsActive())
		{
			CancelSkillSpec(Spec, Ignore);
		}
	}
}
