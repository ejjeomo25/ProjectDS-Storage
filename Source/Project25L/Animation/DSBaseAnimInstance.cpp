//Default
#include "Animation/DSBaseAnimInstance.h"

//UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UDSBaseAnimInstance::UDSBaseAnimInstance()
	:Owner(nullptr)
	, Movement(nullptr)
	, Velocity(FVector::ZeroVector)
	, GroundSpeed(0.f)
	, bisIdle(false)
	, bIsCrouching(false)
	, bIsFalling(false)
	, bIsJumping(false)
	, bShouldMove(false)
	, MovingThreshould(3.0f)
	, JumpingThreshould(30.f)
{
	
}

void UDSBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());


	if (IsValid(Owner))
	{
		Movement = Owner->GetCharacterMovement();
	}

}

void UDSBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(Movement))
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); 
		bisIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling && (Velocity.Z > JumpingThreshould);
		bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();

		FVector Acceleration = Movement->GetCurrentAcceleration();
		bool bHasInput = !Acceleration.IsNearlyZero();
		bShouldMove = (GroundSpeed > MovingThreshould) && bHasInput;
	}
	
}
