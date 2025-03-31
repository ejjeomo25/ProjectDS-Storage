// Default
#include "Skill/Girl/DSPrimarySkill.h"

// UH
#include "GameFramework/HUD.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"
#include "Weapon/DSWeapon.h"
#include "Components/Skill/DSSkillControlComponent.h"

UDSPrimarySkill::UDSPrimarySkill(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	bSkillHasCooltime = false;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

void UDSPrimarySkill::CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::CallActivateSkill(Handle, ActorInfo);
	
}

void UDSPrimarySkill::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr != ActorInfo)
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			const ADSWeapon* Weapon = Character->GetWeapon();
			
			if (IsValid(Weapon))
			{
				//임시로 지정
				Weapon->PrimaryAttack(AutoAimAngle, 400.f);
			}
		}
	}

	EndSkill(Handle, ActorInfo, true, true);
}

void UDSPrimarySkill::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

}
