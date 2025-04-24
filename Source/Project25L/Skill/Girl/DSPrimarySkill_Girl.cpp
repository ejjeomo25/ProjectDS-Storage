// Default
#include "Skill/Girl/DSPrimarySkill_Girl.h"

// UH
#include "GameFramework/HUD.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "Weapon/DSGun.h"

#include "DSLogChannels.h"
UDSPrimarySkill_Girl::UDSPrimarySkill_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	bSkillHasCooltime = false;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

void UDSPrimarySkill_Girl::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::CallActivateSkill(Handle, ActorInfo);
	
}

void UDSPrimarySkill_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			const ADSWeapon* Weapon = Cast<ADSWeapon>(Character->GetWeapon());
			
			if (false == IsValid(Weapon))
			{
				return;
			}

			Weapon->AttackPrimarySkill(AutoAimAngle, 400.f);
		}

	}

	EndSkill(Handle, ActorInfo, true, false);
}

void UDSPrimarySkill_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);
	
}
