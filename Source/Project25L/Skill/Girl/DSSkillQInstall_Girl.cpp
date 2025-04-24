// Default
#include "Skill/Girl/DSSkillQInstall_Girl.h"

// UE
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Weapon/DSWeapon.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "DSLogChannels.h"

UDSSkillQInstall_Girl::UDSSkillQInstall_Girl(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetLocation()
	, bIsStarted(false)
	, Weapon(nullptr)
	, NiagaraEffect(nullptr)
{
	bSkillHasCooltime = true;
	bSkillHasDuration = false;
	bRetriggerInstancedSkill = false; //한번만 실행될 예정
	bCanTick = true;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalOnly;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}


void UDSSkillQInstall_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);
	//스킬 누르는 순간, VFX를 스폰한다.

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());
	if (false == IsValid(Character))
	{
		return;
	}

	Weapon = Character->GetWeapon();

	if (false == IsValid(Weapon))
	{
		return;
	}
	
	TargetLocation = Weapon->GetFinalDestination(AutoAimAngle, 400.f);


	if (Effects.Contains(ESkillActivationStatus::Before))
	{
		UWorld* World = Character->GetWorld();
		check(World);

		NiagaraEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Effects[ESkillActivationStatus::Before].LoadSynchronous(), TargetLocation);
	}

	bIsStarted = true;
}

void UDSSkillQInstall_Girl::Tick(float DeltaSeconds)
{
	if (bIsStarted)
	{
		DS_LOG(DSSkillLog, Log, TEXT("SkillQInstall_Girl %lf"), DeltaSeconds);

		if (IsValid(Weapon))
		{
			if (IsValid(NiagaraEffect))
			{
				TargetLocation = Weapon->GetFinalDestination(AutoAimAngle, 400.f);

				NiagaraEffect->SetRelativeLocation(TargetLocation);
			}
		}
	}
}

void UDSSkillQInstall_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	DS_LOG(DSSkillLog, Log, TEXT("EndSkill %lf"));
}
