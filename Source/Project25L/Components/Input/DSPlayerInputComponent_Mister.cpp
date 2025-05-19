// Default
#include "Components/Input/DSPlayerInputComponent_Mister.h"

// Game
#include "Input/DSInputComponent.h"
#include "GameData/GameplayTag/DSGameplayTags.h"

void UDSPlayerInputComponent_Mister::SetupInputComponent(UInputComponent* InputComponent)
{
	Super::SetupInputComponent(InputComponent);

	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_SecondarySkill, this,
			&UDSPlayerInputComponent_Mister::Input_Weapon_Attack_Started, &UDSPlayerInputComponent_Mister::Input_Weapon_Attack_Completed);
	}
}
