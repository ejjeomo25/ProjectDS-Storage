// Default
#include "Character/Enemies/DSNonCharacter.h"

// UE

// Game
#include "AI/DSAIControllerBase.h"

ADSNonCharacter::ADSNonCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ADSAIControllerBase::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
