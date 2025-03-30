#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Animation/DSBaseAnimInstance.h"

// UHT
#include "DSArmedAnimInstance.generated.h"


UCLASS()
class PROJECT25L_API UDSArmedAnimInstance : public UDSBaseAnimInstance
{
	GENERATED_BODY()
	

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsEquipped : 1;
};
