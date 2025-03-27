// Default
#include "Animation/DSGunAnimInstance.h"

// Game
#include "Item/Weapon/DSWeapon.h"

void UDSGunAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ADSWeapon>(GetOwningActor());
}

void UDSGunAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}
