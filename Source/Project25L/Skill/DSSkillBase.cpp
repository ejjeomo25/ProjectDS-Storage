// Default
#include "Skill/DSSkillBase.h"

// UE
#include "Gameframework/characterMovementComponent.h"
#include "Gameframework/Character.h"

// Game
#include "DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "System/DSEnums.h"

#include "GameData/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"

UDSSkillBase::UDSSkillBase()
: ReplicationPolicy(ESkillReplicationPolicy::ReplicateYes), InstancingPolicy(ESkillInstancingPolicy::InstancedPerActor), NetExecutionPolicy(ESkillNetExecutionPolicy::ServerInitiated),
CurrentActorInfo(nullptr), bSkillHasCooltime(true)
{
	StartWorldTime = -100.f;
}

UDSSkillBase::UDSSkillBase(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer), 
ReplicationPolicy(ESkillReplicationPolicy::ReplicateYes), InstancingPolicy(ESkillInstancingPolicy::InstancedPerActor), NetExecutionPolicy(ESkillNetExecutionPolicy::ServerInitiated),
CurrentActorInfo(nullptr), bSkillHasCooltime(true)
{
	StartWorldTime = -100.f;
}

void UDSSkillBase::SetRemoteInstanceHasEnded()
{
}

void UDSSkillBase::NotifyAvatarDestroyed()
{
    OnSkillDeactivated(*this);
}

// 스킬이 SkillControlComponent에 등록될 때 호출됨
void UDSSkillBase::OnAddSkill(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec)
{
	
	// UE_LOG(LogTemp, Log, TEXT("Skill %s added. Spec: %s"), *GetName(), *Spec.GetDebugString());
	// ActorInfo가 유효하다면 캐시
	if (ActorInfo)
	{
		SetCurrentActorInfo(Spec.Handle, ActorInfo);

		OnSkillInitialized(*this);
	}
}

void UDSSkillBase::OnRemoveSkill(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec)
{
	DS_LOG(DSSkillLog, Warning, TEXT("OnRemoveSkill"));
}

void UDSSkillBase::OnAvatarSet(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec)
{
	// Projects may want to initiate passives or do other "BeginPlay" type of logic here.
}

void UDSSkillBase::PostNetInit()
{
	if (CurrentActorInfo == nullptr)
	{
		AActor* OwnerActor = Cast<AActor>(GetOuter());
		if (ensure(OwnerActor))
		{
			UDSSkillControlComponent* SkillControlComponent = OwnerActor->FindComponentByClass<UDSSkillControlComponent>();
			if (ensure(SkillControlComponent))
			{
				CurrentActorInfo = SkillControlComponent->SkillActorInfo.Get();
			}
		}
	}
}
bool UDSSkillBase::IsInstantiated() const
{
	return !HasAllFlags(RF_ClassDefaultObject);
}

void UDSSkillBase::ConfirmActivateSucceed()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		PostNetInit();
		check(CurrentActorInfo);
		//CurrentActivationInfo.SetActivationConfirmed();

		// OnConfirmDelegate.Broadcast(this);
		// OnConfirmDelegate.Clear();
	}
}

const FDSSkillActorInfo* UDSSkillBase::GetCurrentActorInfo() const
{
	return CurrentActorInfo;
}

void UDSSkillBase::SetCurrentActorInfo(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	if (IsInstantiated())
	{
		CurrentActorInfo = ActorInfo;
		CurrentSpecHandle = Handle;
	}
}

AActor* UDSSkillBase::GetSkillOwner() const
{
    const FDSSkillActorInfo* Info = GetCurrentActorInfo();
    return Info ? Info->SkillOwner.Get() : nullptr;
}

AActor* UDSSkillBase::GetSkillAvatar() const
{
    const FDSSkillActorInfo* Info = GetCurrentActorInfo();
    return Info ? Info->SkillAvatar.Get() : nullptr;
}

UDSSkillControlComponent* UDSSkillBase::GetSkillControlComponent() const
{
    return GetCurrentActorInfo() ? GetCurrentActorInfo()->SkillControlComponent.Get() : nullptr;
}


// -------------------------------------- ISkillLifeCycle --------------------------------------
void UDSSkillBase::OnSkillInitialized(UDSSkillBase& Skill)
{
	if (nullptr == CurrentActorInfo)
	{
		return;
	}

	if(false == IsValid(GetCurrentActorInfo()->SkillOwner.Get()))
	{
		return;
	}

	if (UWorld * OwnerWorld = GetCurrentActorInfo()->SkillOwner->GetWorld())
	{
		UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(OwnerWorld);
	
		check(DataSubsystem);
	
		if (UDataTable* TmpData = DataSubsystem->GetDataTable(EDataTableType::SkillAttributeData))
		{
			if (IsValid(TmpData))
			{
				if (FDSSkillAttribute* Row = TmpData->FindRow<FDSSkillAttribute>(SkillName, SkillName.ToString()))
				{
					MaxCooltime = Row->MaxCooltime;
					DS_LOG(DSSkillLog, Warning, TEXT("[%s] - MaxCooltime : [%f]"),*SkillName.ToString(), MaxCooltime);

				}
			}
		}
	}
}

void UDSSkillBase::OnSkillActivated(UDSSkillBase& Skill)
{
}

void UDSSkillBase::OnSkillDeactivated(UDSSkillBase& Skill)
{
}

// bool UDSSkillBase::TryActivateSkill()
// {
// 	return false;
// }

void UDSSkillBase::PreActivate(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	UDSSkillControlComponent* SkillControlComponent = ActorInfo->SkillControlComponent.Get();

	//Flush any remaining server moves before activating the ability.
	//	Flushing the server moves prevents situations where previously pending move's DeltaTimes are figured into montages that are about to play and update.
	//	When this happened, clients would have a smaller delta time than the server which meant the server would get ahead and receive their notifies before the client, etc.
	//	The system depends on the server not getting ahead, so it's important to send along any previously pending server moves here.
	AActor* const MyActor = ActorInfo->SkillAvatar.Get();
	if (MyActor && !ActorInfo->IsNetAuthority())
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyActor);
		if (MyCharacter)
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MyCharacter->GetMovementComponent());

			if (CharMoveComp)
			{
				CharMoveComp->FlushServerMoves();
			}
		}
	}

	if (GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
	{
		bIsActive = true;
		bIsBlockingOtherSkills = true;
		bIsCancelable = true;
	}

	RemoteInstanceEnded = false;

	SetCurrentActorInfo(Handle, ActorInfo);

	SkillControlComponent->NotifySkillActivated(Handle, this);

	// Spec's active count must be incremented after applying blockor cancel tags, otherwise the ability runs the risk of cancelling itself inadvertantly before it completely activates.
	FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("PreActivate called with a valid handle but no matching ability spec was found. Handle: %s ASC: %s. AvatarActor: %s"), *Handle.ToString(), *(SkillControlComponent->GetPathName()), *GetNameSafe(SkillControlComponent->GetAvatarActor_Direct()));
		return;
	}

	// make sure we do not incur a roll over if we go over the uint8 max, this will need to be updated if the var size changes
	if (LIKELY(Spec->ActiveCount < UINT8_MAX))
	{
		Spec->ActiveCount++;
	}
	else
	{
		DS_LOG(DSSkillLog, Warning, TEXT("PreActivate %s called when the Spec->ActiveCount (%d) >= UINT8_MAX"), *GetName(), (int32)Spec->ActiveCount);
	}
}

void UDSSkillBase::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	DS_LOG(DSSkillLog, Warning, TEXT("CallActivateSkill Function : InputID[%d]"), ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle)->InputID);
	PreActivate(Handle, ActorInfo);
	ActivateSkill(Handle, ActorInfo);
}

void UDSSkillBase::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	DS_LOG(DSSkillLog, Warning, TEXT("ActivateSkill Function : InputID[%d]"), ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle)->InputID);
	CommitSkill(Handle, ActorInfo);
}

bool UDSSkillBase::CommitSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	if(!CommitCheck(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CommitCheck returning false"));
		return false; 
	}

	DS_LOG(DSSkillLog, Warning, TEXT("CommitSkill Function : InputID[%d]"), ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle)->InputID);

	ApplyCooldown(ActorInfo);
	ApplyCost(ActorInfo);

	// Broadcast this commitment
	ActorInfo->SkillControlComponent->NotifySkillCommit(this);
	return true;
}

void UDSSkillBase::CancelSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateCancelSkill)
{
	if (CanBeCanceled())
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CancelSkill Function : InputID[%d]"), ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle)->InputID);
		// Replicate the the server/client if needed
		if (bReplicateCancelSkill && ActorInfo && ActorInfo->SkillControlComponent.IsValid())
		{
			ActorInfo->SkillControlComponent->ReplicateEndOrCancelSkill(Handle, this, true);
		}

		// End the ability but don't replicate it, we replicate the CancelSkill call directly
		bool bReplicateEndSkill = false;
		bool bWasCancelled = true;

		EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);
	}
}

bool UDSSkillBase::IsEndSkillValid(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	// Protect against EndSkill being called multiple times
	// Ending an SkillState may cause this to be invoked again
	if ((bIsActive == false || bIsSkillEnding == true) && GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due to EndSkill being called multiple times"), *GetName());

		return false;
	}

	// check if ability has valid owner
	UDSSkillControlComponent* SkillControlComponent = ActorInfo ? ActorInfo->SkillControlComponent.Get() : nullptr;
	if (SkillControlComponent == nullptr)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due to SkillSystemComponent being invalid"), *GetName());
		return false;
	}

	// check to see if this is an NonInstanced or if the ability is active.
	const FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);
	const bool bIsSpecActive = Spec ? Spec->IsActive() : IsActive();

	if (!bIsSpecActive)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("IsEndSkillValid returning false on Skill %s due spec not being active"), *GetName());
		return false;
	}

	return true;
}

void UDSSkillBase::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled)
{
	if (IsEndSkillValid(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("EndSkill Function : InputID[%d]"), ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle)->InputID);

		// if (ScopeLockCount > 0)
		// {
		// 	UE_LOG(LogSkillSystem, Verbose, TEXT("Attempting to end Skill %s but ScopeLockCount was greater than 0, adding end to the WaitingToExecute Array"), *GetName());
		// 	WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplaySkill::EndSkill, Handle, ActorInfo, ActivationInfo, bReplicateEndSkill, bWasCancelled));
		// 	return;
		// }

		if (GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
		{
			bIsSkillEnding = true;
		}

		// Give blueprint a chance to react
		// K2_OnEndSkill(bWasCancelled);

		// Protect against blueprint causing us to EndSkill already
		if (bIsActive == false && GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
		{
			return;
		}

		// Stop any timers or latent actions for the ability
		UWorld* MyWorld = GetWorld();
		if (MyWorld)
		{
			MyWorld->GetLatentActionManager().RemoveActionsForObject(this);
			// if (FSkillSystemTweaks::ClearSkillTimers)
			{
				MyWorld->GetTimerManager().ClearAllTimersForObject(this);
			}
		}

		// Execute our delegate and unbind it, as we are no longer active and listeners can re-register when we become active again.
		// OnSkillEnded.Broadcast(this);
		// OnSkillEnded.Clear();
		// 
		// OnSkillEndedWithData.Broadcast(FSkillEndedData(this, Handle, bReplicateEndSkill, bWasCancelled));
		// OnSkillEndedWithData.Clear();

		if (GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
		{
			bIsActive = false;
			bIsSkillEnding = false;
		}

		// Tell all our tasks that we are finished and they should cleanup
		// for (int32 TaskIdx = ActiveTasks.Num() - 1; TaskIdx >= 0 && ActiveTasks.Num() > 0; --TaskIdx)
		// {
		// 	UGameplayTask* Task = ActiveTasks[TaskIdx];
		// 	if (Task)
		// 	{
		// 		Task->TaskOwnerEnded();
		// 	}
		// }
		// ActiveTasks.Reset();	// Empty the array but don't resize memory, since this object is probably going to be destroyed very soon anyways.

		if (UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get())
		{
			if (bReplicateEndSkill)
			{
				SkillControlComponent->ReplicateEndOrCancelSkill(Handle, /*ActivationInfo,*/ this, false);
			}

			// Remove tags we added to owner
			// SkillControlComponent->RemoveLooseGameplayTags(ActivationOwnedTags);

			// if (USkillSystemGlobals::Get().ShouldReplicateActivationOwnedTags())
			// {
			// 	SkillSystemComponent->RemoveReplicatedLooseGameplayTags(ActivationOwnedTags);
			// }

			// Remove tracked GameplayCues that we added
			// for (FGameplayTag& GameplayCueTag : TrackedGameplayCues)
			// {
			// 	SkillSystemComponent->RemoveGameplayCue(GameplayCueTag);
			// }
			// TrackedGameplayCues.Empty();

			// if (CanBeCanceled())
			// {
			// 	// If we're still cancelable, cancel it now
			// 	SkillControlComponent->HandleChangeSkillCanBeCanceled(SkillTags, this, false);
			// }

			if (bIsBlockingOtherSkills)
			{
				// If we're still blocking other abilities, cancel now
				// SkillControlComponent->ApplySkillBlockAndCancelTags(SkillTags, this, false, BlockAbilitiesWithTag, false, CancelAbilitiesWithTag);
			}

			// SkillControlComponent->ClearSkillReplicatedDataCache(Handle, CurrentActivationInfo);

			// Tell owning SkillSystemComponent that we ended so it can do stuff (including MarkPendingKill us)
			SkillControlComponent->NotifySkillEnded(Handle, this, bWasCancelled);
		}

		if (IsInstantiated())
		{
			// CurrentEventData = FGameplayEventData{};
		}
	}
}


bool UDSSkillBase::CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	AActor* const AvatarActor = ActorInfo ? ActorInfo->SkillAvatar.Get() : nullptr;
	if (AvatarActor == nullptr || !ShouldActivateSkill(AvatarActor->GetLocalRole()))
	{
		return false;
	}

	UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	if (!SkillControlComponent)
	{
		return false;
	}

	FDSSkillSpec* Spec = SkillControlComponent->FindSkillSpecFromHandle(Handle);
	if (!Spec)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CanActivateSkill%s failed, called with invalid Handle"), *GetName());
		return false;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to Cooldown"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cooldown"), *GetNameSafe(Spec->Skill));

		return false;
	}

	if(!CheckCost(Handle,ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to Cost"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cost"), *GetNameSafe(Spec->Skill));

		return false;
	}

	if (SkillControlComponent->IsSkillInputBlocked(Spec->InputID))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("%s: %s could not be activated due to blocked input ID %d"), *GetNameSafe(ActorInfo->SkillOwner.Get()), *GetNameSafe(Spec->Skill));
		DS_LOG(DSSkillLog, Warning, TEXT("%s could not be activated due to Cost"), *GetNameSafe(Spec->Skill));
		return false;
	}

	DS_LOG(DSSkillLog, Warning, TEXT("CanActivateSkill Function : InputID[%d]"), Spec->InputID);

	return true;
}


bool UDSSkillBase::ShouldActivateSkill(ENetRole Role) const
{
	return Role != ROLE_SimulatedProxy &&
		(Role == ROLE_Authority || (NetSecurityPolicy != ESkillNetSecurityPolicy::ServerOnly && NetSecurityPolicy != ESkillNetSecurityPolicy::ServerOnlyExecution));	// Don't violate security policy if we're not the server
}

void UDSSkillBase::SetShouldBlockOtherSkills(bool bShouldBlockSkills)
{
/*
	if (bIsActive && GetInstancingPolicy() != EGameplaySkillInstancingPolicy::NonInstanced && bShouldBlockAbilities != bIsBlockingOtherAbilities)
	{
		bIsBlockingOtherAbilities = bShouldBlockAbilities;

		USkillSystemComponent* Comp = CurrentActorInfo->SkillSystemComponent.Get();
		if (Comp)
		{
			Comp->ApplySkillBlockAndCancelTags(SkillTags, this, bIsBlockingOtherAbilities, BlockAbilitiesWithTag, false, CancelAbilitiesWithTag);
		}
	}
*/
}

bool UDSSkillBase::CanBeCanceled() const
{
	if (GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
	{
		return bIsCancelable;
	}

	// Non instanced are always cancelable
	return true;
}

void UDSSkillBase::SetCanBeCanceled(bool bCanBeCanceled)
{
	if (GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced && bCanBeCanceled != bIsCancelable)
	{
		bIsCancelable = bCanBeCanceled;

		// UDSSkillControlComponent* SkillControlComponent = CurrentActorInfo->SkillControlComponent.Get();
		// if (SkillControlComponent)
		// {
		// 	Comp->HandleChangeSkillCanBeCanceled(SkillTags, this, bCanBeCanceled);
		// }
	}
}

bool UDSSkillBase::CommitCheck(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	/**
	 *	Checks if we can (still) commit this ability. There are some subtleties here.
	 *		-An ability can start activating, play an animation, wait for a user confirmation/target data, and then actually commit
	 *		-Commit = spend resources/cooldowns. It's possible the source has changed state since it started activation, so a commit may fail.
	 *		-We don't want to just call CanActivateSkill() since right now that also checks things like input inhibition.
	 *			-E.g., its possible the act of starting your ability makes it no longer activatable (CanActivateSkill() may be false if called here).
	 */

	const bool bValidHandle = Handle.IsValid();
	const bool bValidActorInfoPieces = (ActorInfo && (ActorInfo->SkillControlComponent != nullptr));
	const bool bValidSpecFound = bValidActorInfoPieces && (ActorInfo->SkillControlComponent->FindSkillSpecFromHandle(Handle) != nullptr);

	// Ensure that the ability spec is even valid before trying to process the commit
	if (!bValidHandle || !bValidActorInfoPieces || !bValidSpecFound)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("UGameplaySkill::CommitCheck provided an invalid handle or actor info or couldn't find ability spec: %s Handle Valid: %d ActorInfo Valid: %d Spec Not Found: %d"), *GetName(), bValidHandle, bValidActorInfoPieces, bValidSpecFound);
		return false;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CheckCooldown  false"));
		return false;
	}

	if (!CheckCost(Handle, ActorInfo))
	{
		DS_LOG(DSSkillLog, Warning, TEXT("CheckCost  false"));
		return false;
	}

	return true;
}

bool UDSSkillBase::IsActive() const
{
	// Only Instanced-Per-Actor abilities persist between activations
	if (GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor)
	{
		return bIsActive;
	}

	// this should not be called on NonInstanced warn about it, Should call IsActive on the ability spec instead
	if (GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced)
	{
		DS_LOG(DSSkillLog, Warning, TEXT("UGameplayAbility::IsActive() called on %s NonInstanced ability, call IsActive on the Ability Spec instead"), *GetName());
	}

	// NonInstanced and Instanced-Per-Execution abilities are by definition active unless they are pending kill
	return IsValid(this);
}


// -------------------------------------------------------------------------- Cost -------------------------------------------------------------------------- //
bool UDSSkillBase::CheckCost(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	return true;
}

void UDSSkillBase::ApplyCost(const FDSSkillActorInfo* ActorInfo)
{
}
// --------------------------------------------------------------------------

 
// -------------------------------------------------------------------------- Cooldown -------------------------------------------------------------------------- //
void UDSSkillBase::GetCooldownTimeRemainingAndDuration(FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, float& TimeRemaining, float& OutCooldownDuration) const
{
	TimeRemaining = 0.f;
	OutCooldownDuration = 0.f;

	if (bSkillHasCooltime)
	{
		UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
		check(SkillControlComponent != nullptr);

		UWorld* World = SkillControlComponent->GetWorld();
		if (World)
		{
			float CurrentTime = World->GetTimeSeconds();
			float Elapsed = CurrentTime - StartWorldTime;
			OutCooldownDuration = MaxCooltime;
			// 남은 쿨타임은 Duration에서 경과 시간을 뺀 값 (음수이면 0으로 처리)
			TimeRemaining = FMath::Max(MaxCooltime - Elapsed, 0.f);
		}
	}
}

bool UDSSkillBase::CheckCooldown(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const
{
	// 스킬이 쿨타임을 가진다면, 현재 시간과 시작 시간의 차이를 계산
	if (bSkillHasCooltime)
	{
		UDSSkillControlComponent* const SkillControlComponent = ActorInfo->SkillControlComponent.Get();
		check(SkillControlComponent != nullptr);

		UWorld* World = SkillControlComponent->GetWorld();
		if (World)
		{
			float CurrentTime = World->GetTimeSeconds();
			float Elapsed = CurrentTime - StartWorldTime;
			// 아직 쿨타임이 남아 있다면 false를 반환
			if (Elapsed < MaxCooltime)
			{
				DS_LOG(DSSkillLog, Warning, TEXT("CheckCooldown :: Cooldown Not Complete - Elapsed[%f], MaxCooltime[%f]"), Elapsed, MaxCooltime);

				return false;
			}
		}
	}
	// 쿨타임이 없거나, 쿨타임이 끝났으면 true를 반환하여 스킬 사용 가능
	DS_LOG(DSSkillLog, Warning, TEXT("CheckCooldown :: Cooldown Complete"));
	return true;
}

void UDSSkillBase::ApplyCooldown(const FDSSkillActorInfo* ActorInfo)
{
	UDSSkillControlComponent* SkillControlComponent = ActorInfo->SkillControlComponent.Get();
	check(SkillControlComponent != nullptr);

	UWorld* World = SkillControlComponent->GetWorld();

	if (World)
	{
		StartWorldTime = ActorInfo->SkillControlComponent->GetWorld()->GetTimeSeconds();
	
		DS_LOG(DSSkillLog, Warning, TEXT("ApplyCooldown Function : StartWorldTime[%f]"), StartWorldTime);

	}
}
// --------------------------------------------------------------------------
