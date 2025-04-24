#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimNotifies/AnimNotifyState.h"

// UHT
#include "AnimNotifyState_AttackHitCheck.generated.h"

UCLASS()
class PROJECT25L_API UAnimNotifyState_AttackHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_AttackHitCheck();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

protected:

	UPROPERTY(Transient)
	int32 MaxSequenceCount;
	
	/*Hit Checking 용 Socket*/
	UPROPERTY(EditAnywhere, Category = "DSSettings | AnimationSequence")
	FName HitSocket;
};
