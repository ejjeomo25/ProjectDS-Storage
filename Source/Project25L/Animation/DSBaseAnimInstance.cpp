//Default
#include "Animation/DSBaseAnimInstance.h"

//UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include <Character/DSCharacter.h>

UDSBaseAnimInstance::UDSBaseAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.f;
}

void UDSBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}

}

void UDSBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); 
		bisIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
		bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();
	}
	
}

void UDSBaseAnimInstance::PlayPunchMontage()
{
	Montage_Play(PunchMontage, 1.0f);
}


void UDSBaseAnimInstance::AnimNotify_PunchHitCheck()
{
}

void UDSBaseAnimInstance::AnimNotify_NextPunchCheck()
{
}
