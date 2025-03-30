
// Default
#include "Animation/AnimNotify/AnimNotify_AttackHitCheck.h"

// Game
#include "Character/DSCharacterBase.h"
#include "Interface/DSAnimationAttackInterface.h"
#include "DSLogChannels.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if(IDSAnimationAttackInterface* Pawn = Cast<IDSAnimationAttackInterface>(MeshComp->GetOwner()))
		{
			Pawn->AttackHitCheck();
		}
	}
}
