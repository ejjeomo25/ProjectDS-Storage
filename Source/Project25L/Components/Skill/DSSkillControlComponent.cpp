//Default
#include "Components/Skill/DSSkillControlComponent.h"

//UE
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

//Game
#include "Character/DSCharacterBase.h"
#include "DSLogChannels.h"
#include "Skill/Base/DSSkillBase.h"
#include "System/DSEventSystems.h"

#include "System/DSGameUtils.h"

UDSSkillControlComponent::UDSSkillControlComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, AffectedAnimInstanceTag(NAME_None)
, bCachedIsNetSimulated(false)
, bSkillPendingClearAll(false)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;	
	bCachedIsNetSimulated = false;
	bDestroyActiveStateInitiated = false;
	PrimaryComponentTick.bCanEverTick = true;
}

EDataTableType UDSSkillControlComponent::GetSkillDataTableTypeByCharacter(ECharacterType CharacterType, int32 NonCharacterID)
{
	
	if (INDEX_NONE == NonCharacterID)
	{
		//캐릭터 타입
		switch (CharacterType)
		{
		case ECharacterType::Girl:
			return EDataTableType::GirlSkillAttributeData;
		case ECharacterType::Boy:
			return EDataTableType::BoySkillAttributeData;
		case ECharacterType::Mister:
			return EDataTableType::MisterSkillAttributeData;
		}
	}
	else
	{
		//NonCharacter 타입 == Enemy
	}
	
	return EDataTableType();
}


void UDSSkillControlComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UDSSkillControlComponent, ActivatableSkills);
	DOREPLIFETIME(UDSSkillControlComponent, SkillOwnerActor);
	DOREPLIFETIME(UDSSkillControlComponent, SkillAvatarActor);
	DOREPLIFETIME(UDSSkillControlComponent, BlockedSkillBindings);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UDSSkillControlComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	SetIsReplicated(true);


}

void UDSSkillControlComponent::InitializeSkillSpecs()
{
	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_PrimarySkill, FOnSkillPressed());
	DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[FDSTags::GetDSTags().Skill_PrimarySkill], this, &ThisClass::ActivatePrimarySKill);

	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_SecondarySkill, FOnSkillPressed());

	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_Skill1, FOnSkillPressed());
	DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[FDSTags::GetDSTags().Skill_Skill1], this, &ThisClass::ActivateSKill1);

	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_Skill2, FOnSkillPressed());
	DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[FDSTags::GetDSTags().Skill_Skill2], this, &ThisClass::ActivateSKill2);

	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_FarmingSkill, FOnSkillPressed());
	OnSkillPressedEvents.Add(FDSTags::GetDSTags().Skill_UltimateSkill, FOnSkillPressed());


	for (const auto& Skill : Skills)
	{
		FDSSkillSpec SkillSpec(Skill.SkillClass, Skill.SkillType, Skill.SkillTag, UDSGameUtils::GenerateUniqueSkillID());
		SkillSpec.bActivateOnce = true;
		AddSkill(SkillSpec);
	}
}

void UDSSkillControlComponent::OnRegister()
{
	Super::OnRegister();

	// Cached off netrole to avoid constant checking on owning actor
	bCachedIsNetSimulated = IsNetSimulating();
	ActivatableSkills.RegisterWithOwner(this);
	
	if(false == SkillActorInfo.IsValid())
	{
		SkillActorInfo = MakeShared<FDSSkillActorInfo>();
	}

	bDestroyActiveStateInitiated = false;
}

void UDSSkillControlComponent::PreNetReceive()
{
	Super::PreNetReceive();

	if (!bCachedIsNetSimulated)
	{
		bCachedIsNetSimulated = IsNetSimulating();
	}
}

void UDSSkillControlComponent::BeginPlay()
{
	Super::BeginPlay();

	bCachedIsNetSimulated = IsNetSimulating();
	AActor *Owner = GetOwner();

	if (IsValid(Owner))
	{
		InitSkillActorInfo(Owner, Owner);
	}

	InitializeSkillSpecs();
}

void UDSSkillControlComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	DestroyActiveState();

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UDSSkillControlComponent::OnUnregister()
{
	Super::OnUnregister();

	DestroyActiveState();
}

// ---------------------------------------------------------- FindSkillSpec ----------------------------------------------------------

FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromTag(FGameplayTag Tag) const
{
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (false == IsValid(Spec.Skill))
		{
			continue;
		}

		if (Spec.SkillTag == Tag)
		{
			return const_cast<FDSSkillSpec*>(&Spec);
		}
	}

	return nullptr;
}

FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromHandle(FDSSkillSpecHandle Handle) const
{
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (false == IsValid(Spec.Skill))
		{
			continue;
		}

		if (Spec.Handle == Handle)
		{
			return const_cast<FDSSkillSpec*>(&Spec);
		}
	}

	return nullptr;
}

FDSSkillSpec* UDSSkillControlComponent::FindSkillSpecFromClass(TSubclassOf<UDSSkillBase> InSkillClass) const
{
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if(false == IsValid(Spec.Skill))
		{
			continue;
		}

		if (Spec.Skill->GetClass() == InSkillClass)
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
			if (false == IsValid(Spec.Skill))
			{
				continue;
			}

			if (Spec.InputID == InputID)
			{
				return const_cast<FDSSkillSpec*>(&Spec);
			}
		}
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------------------



// ---------------------------------------------------- Add Skill ----------------------------------------------------
UDSSkillBase* UDSSkillControlComponent::CreateNewInstanceOfSkill(FDSSkillSpec& Spec, const UDSSkillBase* Skill)
{
	// 스킬 유효성 체크
	check(Skill);
	// 스킬이 CDO인지 체크
	check(Skill->HasAllFlags(RF_ClassDefaultObject));
	
	if(false == IsValid(GetOwner()))
	{
		return nullptr;
	}

	// Owner를 Outer로 갖는 스킬 인스턴스를 생성
	UDSSkillBase* SkillInstance = NewObject<UDSSkillBase>(GetOwner(), Skill->GetClass());
	
	if(false ==IsValid(SkillInstance))
	{
		return nullptr;
	}

	// 생성된 스킬이 복제 대상이라면
	if (ESkillReplicationPolicy::ReplicateNo != SkillInstance->GetReplicationPolicy())
	{
		// 스펙의 복제 인스턴스 목록에 추가
		Spec.ReplicatedInstances.Add(SkillInstance);
		// 복제 서브 오브젝트로 등록한다.
		AddReplicatedInstancedSkill(SkillInstance);
	}
	// 생성된 스킬이 복제 대상이 아니라면
	else
	{
		// 복제하지 않는 인스턴스 목록에 추가
		Spec.NonReplicatedInstances.Add(SkillInstance);
	}

	return SkillInstance;
}

void UDSSkillControlComponent::AddReplicatedInstancedSkill(UDSSkillBase* Skill)
{
	// 내부 복제 인스턴스 목록 가져오기
	TArray<TObjectPtr<UDSSkillBase>>& ReplicatedAbilities = GetReplicatedInstancedSkills_Mutable();

	// 중복 추가 방지
	if (ReplicatedAbilities.Find(Skill) == INDEX_NONE)
	{
		ReplicatedAbilities.Add(Skill);

		// 복제 준비가 완료되었고, SubObjectList를 사용할 경우 서브오브젝트로 등록
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Skill);
		}
	}
}

// --------------------------------------------------------------------------------------------------------


void UDSSkillControlComponent::MarkSkillSpecDirty(FDSSkillSpec& Spec, bool WasAddOrRemove)
{
	// 서버 측일 경우
	if (IsOwnerActorAuthoritative())
	{
		// 서버 전용 스킬은 변경 사항을 굳이 복제할 필요 없음
		// 단, 스킬을 추가/제거한 경우엔 복제되어야 하므로 예외 처리
		if (false ==(IsValid(Spec.Skill) && ESkillNetExecutionPolicy::ServerOnly == Spec.Skill->GetNetExecutionPolicy()  && false ==WasAddOrRemove))
		{
			// 해당 Spec만 Dirty 처리
			ActivatableSkills.MarkItemDirty(Spec);
		}
	}
	else
	{
		// 클라이언트 예측 실행 중일 경우엔 전체 배열을 Dirty 처리해야 함
		ActivatableSkills.MarkArrayDirty();
	}
}

bool UDSSkillControlComponent::IsOwnerActorAuthoritative() const
{
	return !bCachedIsNetSimulated;
}

void UDSSkillControlComponent::DestroyActiveState()
{
	if (false == bDestroyActiveStateInitiated && ((GetFlags() & RF_BeginDestroyed) == 0))
	{
		bDestroyActiveStateInitiated = true;

		FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();
		
		if (nullptr != ActorInfo && ActorInfo->SkillOwner.IsValid(true) && ActorInfo->SkillControlComponent.IsValid(true))
		{
			CancelAllSkills();
		}

		if (IsOwnerActorAuthoritative())
		{
			CancelAllSkills();
		}
		else
		{
			for (FDSSkillSpec& Spec : ActivatableSkills.Items)
			{
				TArray<UDSSkillBase*> Instances = Spec.GetSkillInstances();
				for (UDSSkillBase* Instance : Instances)
				{
					if (IsValid(Instance))
					{
						if (Instance->IsActive())
						{
							constexpr bool bReplicateEnd = false;
							constexpr bool bWasCancelled = true;
							Instance->EndSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, bReplicateEnd, bWasCancelled);
						}
			
						Instance->OnRemoveSkill(SkillActorInfo.Get(), Spec);
						Instance->MarkAsGarbage();
					}
				}
			
				Spec.ReplicatedInstances.Empty();
				Spec.NonReplicatedInstances.Empty();
			}
		}
	}
}

void UDSSkillControlComponent::ForceCancelSkillDueToReplication(UDSSkillBase* Instance)
{
	check(Instance);

	// Since this was a remote cancel, we should force it through. We do not support 'server says ability was cancelled but client disagrees that it can be'.
	Instance->SetCanBeCanceled(true);
	Instance->CancelSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, false);
}

/**
 * 스킬 활성화 시도 함수
 *  - 내부적으로 CanActivateSkill을 호출하여 조건을 확인
 *  - 스킬 인스턴싱(복제 생성 등)을 처리
 *  - 네트워킹 및 예측 처리(Prediction)를 수행
 *  - 위 모든 과정이 문제 없이 완료되면, 최종적으로 CallActivateSkill가 호출
 */

bool UDSSkillControlComponent::InternalTryActivateSkill(FDSSkillSpecHandle Handle, UDSSkillBase** OutInstancedAbility)
{
	// Handle 유효성 검사 
	if (false == Handle.IsValid())
	{
		return false;
	}

	// Spec 유효성 검사
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (nullptr == Spec)
	{
		return false;
	}

	const FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();

	// SkillActorInfo 유효성 검사
	if (nullptr == ActorInfo || false == ActorInfo->SkillOwner.IsValid() || false == ActorInfo->SkillAvatar.IsValid())
	{
		return false;
	}

	// NetRole 확인
	ENetRole NetMode = ROLE_SimulatedProxy;

	// PlayerController의 Role 사용
	if (APlayerController* PC = ActorInfo->PlayerController.Get())
	{
		NetMode = PC->GetLocalRole();
	}
	// 없을 경우 AvatarActor 기준으로 판단
	else if (AActor* LocalAvatarActor = GetAvatarActor_Direct())
	{
		NetMode = LocalAvatarActor->GetLocalRole();
	}

	// SimulatedProxy는 직접 스킬 발동 불가하므로 return 한다.
	if (NetMode == ROLE_SimulatedProxy)
	{
		return false;
	}

	UDSSkillBase* Skill = Spec->Skill;

	if (false == IsValid(Skill))
	{
		return false;
	}
	
	// 현재 컨트롤 주체가 Local인지 확인
	bool bIsLocal = SkillActorInfo->IsLocallyControlled();

	if (false == bIsLocal)
	{
		// LocalOnly, LocalPredicted 스킬은 로컬에서만 실행 가능하기 때문에 Local이 아닌 경우 실행 불가능하다.
		if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalOnly || (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted))
		{
			NotifySkillFailed(Handle, Skill);
			return false;
		}
	}

	// ServerOnly, ServerInitiated 스킬은 서버에서만 실행 가능
	if (NetMode != ROLE_Authority && (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated))
	{
		NotifySkillFailed(Handle, Skill);
		return false;
	}

	// Skill 인스턴스 가져오기
	// InstancedSkill일 경우 인스턴스 존재 여부 확인
	UDSSkillBase* InstancedSkill = Spec->GetPrimaryInstance();
	UDSSkillBase* SkillSource = InstancedSkill ? InstancedSkill : Skill;

	if (false == SkillSource->CanActivateSkill(Handle, ActorInfo))
	{
		NotifySkillFailed(Handle, SkillSource);
		return false;
	}

	if (ESkillInstancingPolicy::InstancedPerActor == Skill->GetInstancingPolicy() )
	{
		// InstancedPerActor 스킬인데 이미 활성화된 경우
		if (true == Spec->IsActive())
		{
			// 재발동 허용
			if ( true == Skill->bRetriggerInstancedSkill && true == IsValid(InstancedSkill) )
			{
				bool bReplicateEndAbility = true;
				bool bWasCancelled = false;
				// 기존 인스턴스 강제 종료
				InstancedSkill->EndSkill(Handle, ActorInfo, bReplicateEndAbility, bWasCancelled);
			}
			else
			{
				return false;
			}
		}
	}

	// InstancedPerActor인데 인스턴스가 없으면 비정상
	if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor && false ==IsValid(InstancedSkill))
	{
		return false;
	}

	// SkillSpec의 ActivationInfo을 설정한다.
	Spec->ActivationInfo = FDSSkillActivationInfo(ActorInfo->SkillOwner.Get());
	FDSSkillActivationInfo& ActivationInfo = Spec->ActivationInfo;

	// 스킬의 네트워크 실행 정책이 클라이언트에서만 실행이거나, 현재 실행 주체가 서버일 경우
	if (ESkillNetExecutionPolicy::LocalOnly == Skill->GetNetExecutionPolicy() || (ROLE_Authority == NetMode))
	{
		// ----------------------------------------------
		// 로컬 컨트롤이 아니고, 서버 전용 스킬이 아닐 때만
		// 클라이언트에게 스킬이 성공적으로 실행되었음을 알린다.
		// ----------------------------------------------
		if (false == bIsLocal && ESkillNetExecutionPolicy::ServerOnly != Skill->GetNetExecutionPolicy())
		{
			// 서버에서 클라이언트로 스킬을 실행하도록 RPC 함수를 실행한다.
			ClientRPC_ActivateSkillSucceed(Handle);
			// ActivationInfo.bCanBeEndedByOtherInstance = Skill->bServerRespectsRemoteSkillCancellation;
		}

		// ----------------------------------------------
		//	실제 스킬 실행 호출 
		// ----------------------------------------------

		// 스킬이 실행할 때마다 생성된다면, 매번 새 인스턴스를 생성해야 한다.
		if (ESkillInstancingPolicy::InstancedPerExecution == Skill->GetInstancingPolicy())
		{
			// 새 스킬 인스턴스를 생성하고 실행
			InstancedSkill = CreateNewInstanceOfSkill(*Spec, Skill);
			InstancedSkill->CallActivateSkill(Handle, ActorInfo);
		}
		else
		{
			// 인스턴싱이 없는 경우 (NonInstanced 또는 CDO 기반)
			Skill->CallActivateSkill(Handle, ActorInfo);
		}
	}
	else if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted)
	{
		// ---------------------------------------------
		// 클라이언트 예측 실행 스킬: 네트워크 동기화를 위한 사전 처리
		// ---------------------------------------------

		// 서버가 아닌 로컬 클라이언트에서 실행되는 경우
		if (false == ActorInfo->IsNetAuthority())
		{
			// 아바타가 캐릭터일 경우
			ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->SkillAvatar.Get());
			if (IsValid(AvatarCharacter))
			{
				// 캐릭터의 이동 컴포넌트 가져오기
				UCharacterMovementComponent* AvatarCharMoveComp = Cast<UCharacterMovementComponent>(AvatarCharacter->GetMovementComponent());
				if (IsValid(AvatarCharMoveComp))
				{
					// 클라이언트가 서버에 전송 예정인 이동 입력 큐를 모두 정리함
					// - 이렇게 하면 이후 전송될 Skill 관련 RPC가 항상 이동보다 뒤에 도착하지 않도록 보장됨
					// - 루트모션이나 스킬에 의한 이동 영향 발생 시, 서버 위치 보정을 막기 위해 필요
					AvatarCharMoveComp->FlushServerMoves();
				}
			}
		}
		
		// 클라이언트에서 실행 중이고, 해당 컨트롤러가 로컬 컨트롤 권한을 갖고 있을 때, 복제 불가 스킬은 서버에서 의미 없음
		// if ( false ==ActorInfo->IsNetAuthority() && true == ActorInfo->IsLocallyControlled() &&  ESkillReplicationPolicy::ReplicateNo != Skill->GetReplicationPolicy())                                                
		// {
		// 	CallServerTryActivateSkill(Handle, Spec->InputPressed);
		// }

		CallServerTryActivateSkill(Handle, Spec->InputPressed);
		
		// 스킬이 실행할 때마다 생성된다면
		if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
		{
			// 스킬이 복제 대상이 아니라면
			if (Skill->GetReplicationPolicy() == ESkillReplicationPolicy::ReplicateNo)
			{
				// 스킬의 인스턴스를 생성하고, 실행한다.
				InstancedSkill = CreateNewInstanceOfSkill(*Spec, Skill);
				InstancedSkill->CallActivateSkill(Handle, ActorInfo);
			}
		}
		else
		{
			// 인스턴스가 없다면 스킬을 실핸한다.
			Skill ->CallActivateSkill(Handle, ActorInfo);
		}
	}

	// 스킬 인스턴스가 유효한 경우
	if (true == IsValid(InstancedSkill))
	{
		// 인스턴스 반환 포인터가 존재하면 결과값으로 설정해줌
		if (OutInstancedAbility)
		{
			*OutInstancedAbility = InstancedSkill;
		}
	}

	// 스킬 스펙이 수정되었음을 표시하여 복제 동기화를 트리거함
	MarkSkillSpecDirty(*Spec);
	
	// 스킬 Duration이 끝나면 EndSkill 호출
	if (true == IsValid(InstancedSkill))
	{
		if (true == InstancedSkill->bSkillHasDuration)
		{
			ScheduleSkillEnd(Handle, InstancedSkill->SkillDuration, true);
		}
	}
	
	if (true == IsValid(Skill))
	{
		if (true == Skill->bSkillHasDuration)
		{
			ScheduleSkillEnd(Handle, Skill->SkillDuration, false);
		}
	}

	return true;
}

void UDSSkillControlComponent::ScheduleSkillEnd(FDSSkillSpecHandle Handle, float Duration, bool bReplicateEndSkill)
{
	if (Duration <= 0.f) 
	{
		return;
	}

	UWorld* World = GetWorld();
	if (false ==IsValid(World)) 
	{
		return;
	}

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, Handle, bReplicateEndSkill]()
		{
			FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
			if (nullptr == Spec) 
			{
				return;
			}
			
			if(false == IsValid(Spec->Skill))
			{
				return;
			}

			const bool bWasCancelled = false;
			RemoteEndOrCancelSkill(Handle, bWasCancelled);
		});

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		TimerDel,
		Duration,   // Duration 뒤에 실행
		false       // 반복 실행 아님
	);
}

void UDSSkillControlComponent::OnRep_SkillOwnerActor()
{
	check(SkillActorInfo.IsValid());

	AActor* LocalOwnerActor = GetOwnerActor();
	AActor* LocalAvatarActor = GetAvatarActor_Direct();

	if (LocalOwnerActor != SkillActorInfo->SkillOwner || LocalAvatarActor != SkillActorInfo->SkillAvatar)
	{
		if (true ==IsValid(LocalOwnerActor))
		{
			InitSkillActorInfo(LocalOwnerActor, LocalAvatarActor);
		}
		else
		{
			ClearActorInfo();
		}
	}
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

FGameplayTagContainer UDSSkillControlComponent::GetActiveSkillTags() const
{
	FGameplayTagContainer Result;
	for (const FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		if (Spec.IsActive() && Spec.Skill)
		{
			Result.AddTag(Spec.SkillTag); 
		}
	}
	return Result;
}


void UDSSkillControlComponent::OnRep_ActivateSkills()
{
	for (FDSSkillSpec& Spec : ActivatableSkills.Items)
	{
		const UDSSkillBase* SpecSkill = Spec.Skill;
		
		if (false == IsValid(SpecSkill))
		{
			// 스킬이 아직 로드되지 않은 경우에
			//  일정 시간 뒤에 다시 이 함수를 재호출하여 재검사하도록 타이머 설정
			GetWorld()->GetTimerManager().SetTimer(OnRep_ActivateSkillsTimerHandle, this, &UDSSkillControlComponent::OnRep_ActivateSkills, 0.5);
			return;
		}
	}

	CheckForClearedSkills();

	// 서버에서 활성화 명령이 왔지만, 클라이언트에서 아직 완전히 처리되지 않은 스킬 리스트를 순회
	for (const FPendingSkillInfo& PendingAbilityInfo : PendingServerActivatedSkills)
	{
		// 아직 부분적으로도 활성화되지 않은 경우에만 처리
		if (false == PendingAbilityInfo.bPartiallyActivated)
		{
			// 클라이언트에서 해당 스킬을 TryActivate 
			ClientRPC_TryActivateSkill(PendingAbilityInfo.Handle);
		}
	}

	// 모든 Pending 처리 완료 후 리스트 초기화
	PendingServerActivatedSkills.Empty();
}

void UDSSkillControlComponent::OnAddSkill(FDSSkillSpec& Spec)
{
	if (false == IsValid(Spec.Skill))
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

	// 이미 SkillSpec 대해 생성된 Primary Instance가 있다면, 그 인스턴스의 OnAddSkill를 호출한다.
	UDSSkillBase* PrimaryInstance = Spec.GetPrimaryInstance();
	if (PrimaryInstance)
	{
		PrimaryInstance->OnAddSkill(SkillActorInfo.Get(),Spec.SkillType, Spec);
	}
	else
	{
		Spec.Skill->OnAddSkill(SkillActorInfo.Get(),Spec.SkillType, Spec);
	}
}

void UDSSkillControlComponent::OnRemoveSkill(FDSSkillSpec& Spec)
{
	if (false == IsValid(Spec.Skill))
	{
		return;
	}

	// 현재 Spec에서 생성된 모든 인스턴스를 가져옴
	TArray<UDSSkillBase*> Instances = Spec.GetSkillInstances();

	for (auto Instance : Instances)
	{
		if (true == IsValid(Instance))
		{
			// 스킬이 현재 활성 상태라면
			if (true == Instance->IsActive())
			{
				// 스킬을 종료함. 종료 동작은 복제되지 않음 (OnRemoveSkill이 따로 복제되므로)
				bool bReplicateEndAbility = false;
				bool bWasCancelled = false;
				Instance->EndSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, bReplicateEndAbility, bWasCancelled);
			}
			// 스킬이 비활성 상태인데, 아직 인스턴스가 살아 있음
			else
			{
				// 서버에서만 삭제 가능. 클라이언트에서 삭제하면 이후 복제에 문제가 생김
				if (GetOwnerRole() == ROLE_Authority)
				{
					RemoveReplicatedInstancedSkill(Instance);
				}

				// 만약 이 스킬이 InstancedPerExecution 정책이라면 직접 GC 대상 표시
				if (Instance->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
				{
					// GC로 제거되도록 표시
					Instance->MarkAsGarbage();
				}
			}
		}
	}

	// 스킬의 대표 인스턴스를 가져옴 (보통 1개만 존재하거나 CDO일 수 있음)
	UDSSkillBase* PrimaryInstance = Spec.GetPrimaryInstance();
	if (true == IsValid(PrimaryInstance))
	{
		// 스킬에서 제거 시 처리해주는 OnRemoveSkill 호출
		PrimaryInstance->OnRemoveSkill(SkillActorInfo.Get(), Spec);

		// 서버일 경우 
		if (GetOwnerRole() == ROLE_Authority)
		{
			// replicated 인스턴스 제거
			RemoveReplicatedInstancedSkill(PrimaryInstance);
		}
		// GC 대상 처리
		PrimaryInstance->MarkAsGarbage();
	}
	else
	{
		// 인스턴스가 없는데 스킬이 활성 상태일 경우 (NonInstanced일 때 발생 가능)
		if (true == Spec.IsActive())
		{
			// // 인스턴싱 정책이 NonInstanced인 경우만 여기 도달해야 함
			if (ensureMsgf(Spec.Skill->GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced, TEXT("We should never have an instanced Skill that is still active by this point. All instances should have EndSkill called just before here.")))
			{
				// 비인스턴스 스킬의 종료 처리 (복제 없이 종료)
				constexpr bool bReplicateEndAbility = false;
				constexpr bool bWasCancelled = false;
				Spec.Skill->EndSkill(Spec.Handle,SkillActorInfo.Get(), bReplicateEndAbility, bWasCancelled);
			}
		}

		// 비인스턴스 스킬의 제거 처리
		Spec.Skill->OnRemoveSkill(SkillActorInfo.Get(), Spec);
	}

	// 남아 있는 인스턴스 목록 비우기
	Spec.ReplicatedInstances.Empty();
	Spec.NonReplicatedInstances.Empty();
}

void UDSSkillControlComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const FDSSkillSpec& ActiveSpec : ActivatableSkills.Items)
	{
		UDSSkillBase* Skill = ActiveSpec.GetPrimaryInstance();

		if (false == IsValid(Skill))
		{
			continue;
		}

		if (false == Skill->bCanTick)
		{
			continue;
		}

		if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalOnly ||
			Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated && GetOwnerRole() == ROLE_Authority ||
			Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted && GetOwnerRole() != ROLE_Authority)
		{
			Skill->Tick(DeltaTime);
		}
	}
}

FDSSkillSpecHandle UDSSkillControlComponent::AddSkill(const FDSSkillSpec& Spec)
{
	if (false == IsValid(Spec.Skill))
	{
		return FDSSkillSpecHandle();
	}

	if (false == IsOwnerActorAuthoritative())
	{
		return FDSSkillSpecHandle();
	}

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

	if (nullptr == Spec)
	{
		return false;
	}

	if (true == Spec->PendingRemove || true == Spec->RemoveAfterActivation)
	{
		return false;
	}

	// 스킬의 유효성 검사
	UDSSkillBase* Skill = Spec->Skill;
	if (false == IsValid(Skill))
	{
		return false;
	}

	const FDSSkillActorInfo* ActorInfo = SkillActorInfo.Get();

	if (nullptr == ActorInfo || false ==ActorInfo->SkillOwner.IsValid() || false == ActorInfo->SkillAvatar.IsValid())
	{
		return false;
	}

	const ENetRole NetMode = ActorInfo->SkillAvatar->GetLocalRole();

	// 이 스킬은 로컬 조작에 의해서만 실행되어야 함
	if (NetMode == ROLE_SimulatedProxy)
	{
		return false;
	}

	// 현재 컨트롤러가 로컬인지 여부(내가 직접 조작하는 캐릭터인지)
	bool bIsLocal = SkillActorInfo->IsLocallyControlled();

	// ------------------------------------------------------------
	// 원격 클라이언트에서 로컬 전용(LocalOnly, LocalPredicted) 스킬을 실행하려 한 경우
	// ------------------------------------------------------------
	if (false ==bIsLocal && (ESkillNetExecutionPolicy::LocalOnly == Skill->GetNetExecutionPolicy() || ESkillNetExecutionPolicy::LocalPredicted == Skill->GetNetExecutionPolicy()))
	{
		// bAllowRemoteActivation이 true라면 서버에서 대신 실행 시도
		if (true == bAllowRemoteActivation)
		{
			ClientRPC_TryActivateSkill(SkillToActivate);
			return true;
		}

		// 로컬이 아닌데 로컬 스킬을 실행하면 실패 처리
		return false;
	}

	// 클라이언트에서 실행 요청한 스킬이 ServerOnly 또는 ServerInitiated인 경우
	// 클라이언트는 직접 실행할 수 없고, 서버에 요청만 가능함
	if (NetMode != ROLE_Authority && (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerOnly || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated))
	{
		// 원격 실행이 허용된 경우
		if (bAllowRemoteActivation)
		{
			// 서버 기준으로 스킬 발동 조건 검사 (쿨다운, 리소스 등)
			if (true == Skill->CanActivateSkill(SkillToActivate, ActorInfo)) 
			{
				// 클라이언트에서 서버로 스킬 실행 요청을 보냄
				CallServerTryActivateSkill(SkillToActivate, Spec->InputPressed); 
				return true;
			}
			else
			{
				// 조건 불충분으로 스킬 발동 실패 → 실패 피드백 전달
				NotifySkillFailed(SkillToActivate, Skill);
				return false;
			}
		}

		return false;
	}
	
	return InternalTryActivateSkill(SkillToActivate);
}



void UDSSkillControlComponent::CallServerTryActivateSkill(FDSSkillSpecHandle SkillToActivate, bool InputPressed)
{
	ServerRPC_TryActivateSkill(SkillToActivate, InputPressed);
}

void UDSSkillControlComponent::CallServerEndSkill(FDSSkillSpecHandle SkillHandle)
{
	ServerRPC_EndSkill(SkillHandle);
}

void UDSSkillControlComponent::ServerRPC_EndSkill_Implementation(FDSSkillSpecHandle SkillToEnd)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(SkillToEnd);

	if (nullptr != Spec && true == IsValid(Spec->Skill) &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
	{
		RemoteEndOrCancelSkill(SkillToEnd, false);
	}
}

bool UDSSkillControlComponent::ServerRPC_EndSkill_Validate(FDSSkillSpecHandle Handle)
{
	return true;
}
void UDSSkillControlComponent::ServerRPC_TryActivateSkill_Implementation(FDSSkillSpecHandle Handle, bool InputPressed)
{
	InternalServerTryActivateSkill(Handle, InputPressed);
}
bool UDSSkillControlComponent::ServerRPC_TryActivateSkill_Validate(FDSSkillSpecHandle SkillToActivate, bool InputPressed)
{
	return true;
}

void UDSSkillControlComponent::ClientRPC_TryActivateSkill_Implementation(FDSSkillSpecHandle Handle)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	
	// Spec이 존재하지 않는 경우 (클라이언트가 아직 해당 스킬을 복제받지 못했을 수 있음)
	if (nullptr == Spec)
	{
		/// 이 상황은 보통 "서버에서 스킬을 추가한 직후, 같은 프레임에 활성화 명령이 왔을 때"
		// 클라이언트가 아직 해당 스킬을 받기 전이기 때문에 발생함
		FPendingSkillInfo AbilityInfo;
		AbilityInfo.Handle = Handle;
		AbilityInfo.bPartiallyActivated = false;

		// 이미 Pending 리스트에 없는 경우에만 추가됨
		// → 재진입 방지: 현재 이 함수가 Pending 목록에서 재호출된 경우엔 중복 추가되지 않음
		PendingServerActivatedSkills.AddUnique(AbilityInfo);

		// 아직 Spec이 없으므로 여기서 종료 → 나중에 OnRep 또는 Tick에서 재시도됨
		return;
	}
	// Spec이 유효하면 → 실제 스킬 활성화를 시도
	InternalTryActivateSkill(Handle);
}

void UDSSkillControlComponent::NotifySkillCommit(UDSSkillBase* Skill)
{

}

void UDSSkillControlComponent::NotifySkillActivated(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill)
{
	
}

void UDSSkillControlComponent::NotifySkillFailed(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill)
{

}


void UDSSkillControlComponent::NotifySkillEnded(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCancelled)
{
	check(Skill);
	
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);

	if (Spec == nullptr)
	{
		return;
	}

	ENetRole OwnerRole = GetOwnerRole();

	if (ensureMsgf(Spec->ActiveCount > 0, TEXT("NotifySkillEnded called when the Spec->ActiveCount <= 0 for skill %s"), *Skill->GetName()))
	{
		Spec->ActiveCount--;
	}

	/** 
	* 만약 스킬이 InstancedPerExecution 타입이거나, Cleanup이 필요한 경우라면,
	* GC 대상으로 표시하고 인스턴스 목록에서 제거한다 (서버에서만 처리)
	*/
	if (Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerExecution)
	{
		// CDO에서는 처리하면 안 됨. 인스턴스 스킬만 처리해야 함.
		check(Skill->HasAnyFlags(RF_ClassDefaultObject) == false);

		// 복제되는 스킬인 경우
		if (Skill->GetReplicationPolicy() != ESkillReplicationPolicy::ReplicateNo)
		{
			// 서버에서만 복제 인스턴스 제거
			if (OwnerRole == ROLE_Authority)
			{
				Spec->ReplicatedInstances.Remove(Skill);
				// 네트워크 동기화된 인스턴스 제거
				RemoveReplicatedInstancedSkill(Skill);
			}
		}
		// 복제되지 않는 스킬인 경우
		else
		{
			// 클라이언트에서도 제거 가능
			Spec->NonReplicatedInstances.Remove(Skill);
		}

		// GC 대상 처리
		Skill->MarkAsGarbage();
	}

	// 서버에서만 수행되어야 한다.
	if (OwnerRole == ROLE_Authority)
	{
		// 스킬이 RemoveAfterActivation 설정되어 있고, 현재 비활성 상태라면
		if (Spec->RemoveAfterActivation && false == Spec->IsActive())
		{
			// 더 이상 활성 인스턴스가 없으므로, 스킬 전체를 제거해도 됨
			ClearSkill(Handle);
		}
		else
		{
			// 아직 제거할 필요는 없지만 상태가 바뀌었으므로 복제 대상으로 표시
			// 다음 Replication에서 상태 동기화됨
			MarkSkillSpecDirty(*Spec);
		}
	}
}

void UDSSkillControlComponent::ReplicateEndOrCancelSkill(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCanceled)
{
	if (Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted || Skill->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::ServerInitiated)
	{
		// 서버일 경우: 클라이언트에게 종료 알림을 보낼 수 있음
		if (GetOwnerRole() == ROLE_Authority)
		{
			// 로컬 컨트롤러가 아닌 클라이언트에게만 종료/취소 알림 전송
			if (false == SkillActorInfo->IsLocallyControlled())
			{
				// 서버에서 클라이언트로 종료/취소 알림 RPC 호출
				if (bWasCanceled)
				{
					ClientRPC_CancelSkill(Handle); 
				}
				else
				{
					ClientRPC_EndSkill(Handle);
				}
			}
		}
		// 클라이언트일 경우: 서버에게 종료/취소를 알릴 수 있는 조건 검사
		else if (Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination && Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
		{
			// 클라이언트에서 서버로 종료/취소 알림 전송 
			if (bWasCanceled)
			{
				ServerRPC_CancelSkill(Handle); 
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
		else
		{
			TArray<UDSSkillBase*> Instances = SkillSpec->GetSkillInstances();
			// 
			for (auto Instance : Instances)
			{
				if(true == IsValid(Instance))
				{
					if(true == bWasCanceled)
					{
						ForceCancelSkillDueToReplication(Instance);
					}
					else
					{
						Instance->EndSkill(Instance->CurrentSpecHandle, Instance->CurrentActorInfo, false, bWasCanceled);
					}
				}
			}
		}
	}
}

void UDSSkillControlComponent::InternalServerTryActivateSkill(FDSSkillSpecHandle Handle, bool InputPressed)
{
#if WITH_SERVER_CODE
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (nullptr == Spec)
	{
		// 클라가 활성화 요청을 보냈지만, 그 전에 서버에서 해당 스킬이 제거된 경우
		DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of skill with invalid SpecHandle!"));
		ClientRPC_ActivateSkillFailed(Handle);
		return;
	}

	const UDSSkillBase* SkillToActivate = Spec->Skill;

	if (!ensure(SkillToActivate))
	{
		DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of unconfigured spec skill!"));
		ClientRPC_ActivateSkillFailed(Handle);
		return;
	}

	// 클라이언트가 실행하면 안 되는 스킬(Net 보안 정책 위반 여부 검사)
	if (SkillToActivate->GetNetSecurityPolicy() == ESkillNetSecurityPolicy::ServerOnlyExecution ||
		SkillToActivate->GetNetSecurityPolicy() == ESkillNetSecurityPolicy::ServerOnly)
	{
		DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of %s due to security policy violation."), *GetNameSafe(SkillToActivate));
		ClientRPC_ActivateSkillFailed(Handle);
		return;
	}

	ensure(SkillActorInfo.IsValid());

	SCOPE_CYCLE_UOBJECT(Skill, SkillToActivate);

	// 입력이 눌렸음을 표시 (Pressed 상태로 변경)
	UDSSkillBase* InstancedSkill = nullptr;
	Spec->InputPressed = true;

	if (InternalTryActivateSkill(Handle,  &InstancedSkill))
	{
		// 성공 시 
		// InternalTryActivateSkill 안에서 클라이언트에게 성공 알림을 자동 전송함
	}
	else
	{
		// 실패 시
		// 클라이언트에게 실패 알림 전송
		DS_NETLOG(DSSkillLog, Warning, TEXT("InternalServerTryActivateSkill. Rejecting ClientActivation of %s. "), *GetNameSafe(Spec->Skill));
		ClientRPC_ActivateSkillFailed(Handle);
		
		// InputPressed 상태 복구
		Spec->InputPressed = false;

		// 상태 변경을 복제 대상으로 표시
		MarkSkillSpecDirty(*Spec);
	}
#endif
}


void UDSSkillControlComponent::ClientRPC_ActivateSkillFailed_Implementation(FDSSkillSpecHandle Handle)
{
	// Find the actual UGameplayAbility		
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (Spec == nullptr)
	{
		return;
	}

	TArray<UDSSkillBase*> Instances = Spec->GetSkillInstances();
	for (UDSSkillBase* Skill : Instances)
	{
		if(true == IsValid(Skill))
		{
			Skill->CurrentActivationInfo.SetActivationRejected();
			Skill->EndSkill(Handle, SkillActorInfo.Get(),true, false);
		}
	}
}

void UDSSkillControlComponent::ClearSkill(const FDSSkillSpecHandle& Handle)
{
	if (false == IsOwnerActorAuthoritative())
	{
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
			return;
		}
	}
}

void UDSSkillControlComponent::ClientRPC_CancelSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	RemoteEndOrCancelSkill(SkillToCancel, true);
}

void UDSSkillControlComponent::ServerRPC_CancelSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(SkillToCancel);

	if (Spec && Spec->Skill &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnlyTermination &&
		Spec->Skill->GetNetSecurityPolicy() != ESkillNetSecurityPolicy::ServerOnly)
	{
		RemoteEndOrCancelSkill(SkillToCancel, true);
	}
}

bool UDSSkillControlComponent::ServerRPC_CancelSkill_Validate(struct FDSSkillSpecHandle SkillToCancel)
{
	return true;
}

void UDSSkillControlComponent::ClientRPC_EndSkill_Implementation(struct FDSSkillSpecHandle SkillToCancel)
{
	RemoteEndOrCancelSkill(SkillToCancel, false);
}


void UDSSkillControlComponent::ClientRPC_ActivateSkillSucceed_Implementation(FDSSkillSpecHandle Handle)
{
	FDSSkillSpec* Spec = FindSkillSpecFromHandle(Handle);
	if (nullptr == Spec)
	{
		FPendingSkillInfo SkillInfo;
		SkillInfo.Handle = Handle;
		SkillInfo.bPartiallyActivated = true;

		// This won't add it if we're currently being called from the pending list
		PendingServerActivatedSkills.AddUnique(SkillInfo);
		return;
	}

	UDSSkillBase* SkillToActivate = Spec->Skill;

	check(SkillToActivate);
	ensure(SkillActorInfo.IsValid());

	Spec->ActivationInfo.SetActivationConfirmed();

	// 클라이언트에서 먼저 실행하고, 서버에서 최종으로 결정하는 경우
	if (SkillToActivate->GetNetExecutionPolicy() == ESkillNetExecutionPolicy::LocalPredicted)
	{
		// 하나의 CDO 공유하는 경우
		if (SkillToActivate->GetInstancingPolicy() == ESkillInstancingPolicy::NonInstanced)
		{	
		}
		// 상태 저장이 가능한 경우
		else
		{
			bool found = false;
			TArray<UDSSkillBase*> Instances = Spec->GetSkillInstances();
			for (UDSSkillBase* LocalSkill : Instances)
			{
				if (true == IsValid(LocalSkill)) 
				{
					LocalSkill->ConfirmActivateSucceed();
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

			if (false == IsValid(InstancedAbility))
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

	bool AvatarChanged = (InAvatarActor != SkillActorInfo->SkillAvatar);

	SkillActorInfo->InitFromActor(InOwnerActor, InAvatarActor, this);
	
	SetOwnerActor(InOwnerActor);

	const AActor* PrevAvatarActor = GetAvatarActor_Direct();
	SetAvatarActor_Direct(InAvatarActor);

	if (true == AvatarChanged)
	{
		for (FDSSkillSpec& Spec : ActivatableSkills.Items)
		{
			if (true == IsValid(Spec.Skill))
			{
				if (ESkillInstancingPolicy::InstancedPerActor == Spec.Skill->GetInstancingPolicy())
				{
					UDSSkillBase* SkillInstance = Spec.GetPrimaryInstance();
					if (true == IsValid(SkillInstance))
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
	if (false == IsOwnerActorAuthoritative())
	{
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

		if (false == IsValid(Skill))
		{
			if (true == IsUsingRegisteredSubObjectList())
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

	// 인스턴싱 정책이 NonInstanced가 아닌 경우
	if (Spec.Skill->GetInstancingPolicy() != ESkillInstancingPolicy::NonInstanced)
	{
		// 인스턴스를 직접 만들어 사용하는 방식이므로, 현재 활성화된 모든 스킬 인스턴스를 가져온다
		TArray<UDSSkillBase*> SkillsToCancel = Spec.GetSkillInstances();
		for (UDSSkillBase* InstanceSkill : SkillsToCancel)
		{
			// 인스턴스가 유효하고, 무시 대상(Ignore)이 아닌 경우에만 취소
			if (true == IsValid(InstanceSkill) && Ignore != InstanceSkill)
			{
				// 해당 인스턴스를 취소 처리. 마지막 인자는 bReplicateCancel = true
				InstanceSkill->CancelSkill(Spec.Handle, ActorInfo, true);
			}
		}
	}
	// 인스턴싱 정책이 NonInstance인 경우
	else
	{
		// 인스턴스가 없으므로, CDO에 대해 직접 취소 처리
		Spec.Skill->CancelSkill(Spec.Handle, ActorInfo, true);
	}
	// 스킬의 상태가 바뀌었으므로 복제 대상 처리
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
		if (true == Spec.IsActive())
		{
			CancelSkillSpec(Spec, Ignore);
		}
	}
}

