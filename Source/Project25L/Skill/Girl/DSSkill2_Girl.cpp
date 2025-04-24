// Default
#include "DSSkill2_Girl.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Weapon/DSGun.h"

#include "DSLogChannels.h"

UDSSkill2_Girl::UDSSkill2_Girl(const FObjectInitializer& ObjectInitalize)
:Super(ObjectInitalize)
{
	bSkillHasCooltime = true;
	bSkillHasDuration = true;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}


void UDSSkill2_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
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
			const float SkillRadisus = 10.f;
			const float SkillDelay = 1.f;
			Weapon->AttackSkill2(AutoAimAngle, 400.f, SkillRadisus);
		}
	}
}

void UDSSkill2_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			ADSWeapon* Weapon = Character->GetWeapon();

			// if (IsValid(Weapon))
			// {
			// 	//임시로 지정
			// 	Weapon->EndAttackSkill2();
			// }
		}
	}
}
