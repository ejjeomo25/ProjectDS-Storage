// Default
#include "Animation/AnimNotify/AnimNotifyState_AttackHitCheck.h"
// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/Animation/DSAttackAnimMetaData.h"

#include "DSLogChannels.h"

UAnimNotifyState_AttackHitCheck::UAnimNotifyState_AttackHitCheck()
	: Super()
	, MaxSequenceCount(0)
{
}

void UAnimNotifyState_AttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(MeshComp->GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UAnimMontage* AnimMontage = Cast<UAnimMontage>(Animation);

	if (IsValid(AnimMontage))
	{
		for (const auto MetaData : AnimMontage->GetMetaData())
		{
			UDSAttackAnimMetaData* AnimMetaData = Cast<UDSAttackAnimMetaData>(MetaData);

			if (IsValid(AnimMetaData))
			{
				MaxSequenceCount = AnimMetaData->GetComboCount();

				Character->AttackHitCheck(HitSocket, AnimMetaData->GetSkillType());
			}
		}	
	}
}

void UAnimNotifyState_AttackHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (false == IsValid(MeshComp))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(MeshComp->GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}
	//콤보 입력 가능
	Character->SetNextComboEnable(MaxSequenceCount);
}
