// Default
#include "Skill/Girl/DSSkillR_Girl.h"

// Game
#include "Character/Characters/DSCharacter.h"

UDSSkillR_Girl::UDSSkillR_Girl(const FObjectInitializer& ObjectInitalize)
	: Super(ObjectInitalize)
{
	bSkillHasDuration = true;
	bRetriggerInstancedSkill = false;
	InstancingPolicy = ESkillInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = ESkillNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = ESkillReplicationPolicy::ReplicateYes;
}

void UDSSkillR_Girl::ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo)
{
	Super::ActivateSkill(Handle, ActorInfo);

	if (nullptr != ActorInfo)
	{
		ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			Character->SetCanFly(true);
		}
	}
}

void UDSSkillR_Girl::EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateEndSkill, bool bWasCancelled)
{
	Super::EndSkill(Handle, ActorInfo, true, true);
	if (nullptr != ActorInfo)
	{
		ADSCharacter* Character = Cast<ADSCharacter>(ActorInfo->SkillOwner.Get());

		if (IsValid(Character))
		{
			Character->SetCanFly(false);
		}
	}
}
