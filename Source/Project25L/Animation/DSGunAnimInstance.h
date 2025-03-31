#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Animation/AnimInstance.h"

// UHT
#include "DSGunAnimInstance.generated.h"

class ADSWeapon;
UCLASS()
class PROJECT25L_API UDSGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	//*****************코드 리뷰 : Transient 변경  ****************************//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ADSWeapon> Owner;

	//*****************코드 리뷰 : uint8 변경  ****************************//
	bool bIsFire;
};
