// Default
#include "Skill/Girl/DSPrimarySkill.h"

// UH
#include "GameFramework/HUD.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"
#include "Item/Weapon/DSWeapon.h"
#include "Skill/DSSkillControlComponent.h"

UDSPrimarySkill::UDSPrimarySkill()
{
	bSkillHasCooltime = false;
	SkillName = FName("PrimarySkill_Girl");

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
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

}

void UDSPrimarySkill::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, bReplicateEndSkill, bWasCancelled);

}
