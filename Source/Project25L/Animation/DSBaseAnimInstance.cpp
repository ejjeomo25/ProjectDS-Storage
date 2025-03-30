//Default
#include "Animation/DSBaseAnimInstance.h"

//UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


//*****************코드 리뷰 : 초기화리스트 사용해서 변수 리셋 ****************************//
UDSBaseAnimInstance::UDSBaseAnimInstance()
{
	MovingThreshould = 3.0f;

	//*****************코드 리뷰 : 점프값 수정해주세요  ****************************//
	JumpingThreshould = 100.f;
}

void UDSBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());

	//*****************코드 리뷰 : IsValid  ****************************//

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}

}

void UDSBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//*****************코드 리뷰 : IsValid  ****************************//
	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); 
		bisIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();

		//*****************코드 리뷰 : 초기종료 &&로 수정해주세요.  ****************************//
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
		bIsCrouching = Owner->GetCharacterMovement()->IsCrouching();
	}
	
}
