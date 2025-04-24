// Default
#include "Skill/Girl/DSSkill1_Girl.h"

// Game
#include "Components/Skill/DSSkillControlComponent_Girl.h"
#include "Character/DSArmedCharacter.h"
#include "Weapon/DSGun.h"

#include "DSLogChannels.h"
UDSSkill1_Girl::UDSSkill1_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
	, TargetLocation()
	, bHasInstalledSkill(false)
{
	bSkillHasCooltime = true;
	bSkillHasDuration = true;
	bRetriggerInstancedSkill = true;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

void UDSSkill1_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());
		
		if (IsValid(Character))
		{
			ADSWeapon* Weapon = Character->GetWeapon();

			if (false == IsValid(Weapon))
			{
				return;
			}
		
			DS_LOG(DSSkillLog, Log, TEXT("Skill1 is Activated %s"), *TargetLocation.ToString());
			Weapon->AttackSkill1(TargetLocation, 200.f, 500.f);
		}
	}
}

void UDSSkill1_Girl::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	PreActivate(Handle, ActorInfo);

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());
	if (false == IsValid(Character))
	{
		return;
	}

	ADSWeapon* Weapon = Character->GetWeapon();

	if (false == IsValid(Weapon))
	{
		return;
	}
	//데칼 스폰
	UDSSkillControlComponent_Girl* DSSkillControlComponent_Girl = Cast<UDSSkillControlComponent_Girl>(ActorInfo->SkillControlComponent);

	if (false == IsValid(DSSkillControlComponent_Girl))
	{
		return;
	}

	if (false == bHasInstalledSkill)
	{
		TargetLocation = Weapon->GetFinalDestination(AutoAimAngle, 400.f);
		DS_LOG(DSSkillLog, Log, TEXT("Skill1 is Wait %s"), *TargetLocation.ToString());

		DrawDebugCircle(
			Character->GetWorld(),
			TargetLocation,                // 중심 좌표 (FVector)
			200.f,            // 반지름 (float)
			32,                    // 세그먼트 수 (원 매끄럽게 할수록 높임)
			FColor::Blue,          // 색상
			false,                 // 영구 표시 여부
			5.0f,                  // 지속 시간
			0,                     // 깊이 우선 순위
			1.0f,                  // 선 두께
			FVector(1, 0, 0),        // Y축 회전 기준 벡터 (보통 X축)
			FVector(0, 1, 0),        // Z축 회전 기준 벡터 (보통 Y축)
			false                  // 3D 회전 원 여부 (false면 평면 원)
		);

		bHasInstalledSkill = true;
		DSSkillControlComponent_Girl->InstallSkillTimer(3.f, false); //타이머가 동작한다.

		EndSkill(Handle, ActorInfo, false, false);
	}
	else
	{
		bHasInstalledSkill = false;
		DSSkillControlComponent_Girl->InstallSkillTimer(-1.f, true); //타이머가 동작한다
		ActivateSkill(Handle, ActorInfo);
	}
}

void UDSSkill1_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			ADSWeapon* Weapon = Character->GetWeapon();

			if (IsValid(Weapon))
			{
				//임시로 지정
				DS_LOG(DSSkillLog, Log, TEXT("Skill1 is Finished"));
				Weapon->EndAttackSkill1();
			}
		}
	}
}

void UDSSkill1_Girl::PostInitProperties()
{
	Super::PostInitProperties();
	bHasInstalledSkill = false;
}

void UDSSkill1_Girl::ExpireSkill()
{
	DS_LOG(DSSkillLog,Log, TEXT("Skill1 is expired"));
	bHasInstalledSkill = false;
	//데칼 삭제
}
