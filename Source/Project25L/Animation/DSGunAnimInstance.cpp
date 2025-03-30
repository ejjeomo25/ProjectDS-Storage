// Default
#include "Animation/DSGunAnimInstance.h"

// Game
#include "Item/Weapon/DSWeapon.h"

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
