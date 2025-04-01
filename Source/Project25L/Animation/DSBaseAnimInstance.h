#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Animation/AnimInstance.h"

//Game

//UHT
#include "DSBaseAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class PROJECT25L_API UDSBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UDSBaseAnimInstance();


protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; 

protected:

	UPROPERTY(Transient)
	TObjectPtr<ACharacter> Owner;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> Movement;

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(Transient, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bisIdle : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsJumping : 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Threshold")
	float MovingThreshold;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Threshold")
	float JumpingThreshold;


};
