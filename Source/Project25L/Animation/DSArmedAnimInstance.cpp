// Default
#include "Animation/DSArmedAnimInstance.h"

// Game
#include "Character/DSArmedCharacter.h"

void UDSArmedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UDSArmedAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(Owner))
	{
		ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(Owner);

		if (IsValid(Character))
		{
			bIsEquipped = Character->GetIsEquipped();
		}
	}
}
