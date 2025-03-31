// Default
#include "Animation/Weapons/DSGunAnimInstance.h"

// Game
#include "Weapon/DSWeapon.h"

//*****************코드 리뷰 : 초기화리스트 사용해서 변수 리셋 ****************************//
void UDSGunAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ADSWeapon>(GetOwningActor());
}

void UDSGunAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


}
