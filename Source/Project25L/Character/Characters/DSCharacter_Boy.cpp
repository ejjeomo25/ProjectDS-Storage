// Default
#include "Character/Characters/DSCharacter_Boy.h"

// Game
#include "Components/Skill/DSSkillControlComponent_Boy.h"
#include "Components/Input/DSPlayerInputComponent_Boy.h"

ADSCharacter_Boy::ADSCharacter_Boy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Boy>(TEXT("SkillControlComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent_Boy>(TEXT("DSPlayerInputComponent"));

}
