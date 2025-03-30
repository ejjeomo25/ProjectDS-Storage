#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotify.h"

// UHT
#include "AnimNotify_AttackHitCheck.generated.h"

UCLASS()
class PROJECT25L_API UAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	public:
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
