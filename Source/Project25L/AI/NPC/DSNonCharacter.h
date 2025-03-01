#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSCharacterBase.h"

// UHT
#include "DSNonCharacter.generated.h"

UCLASS()
class PROJECT25L_API ADSNonCharacter : public ADSCharacterBase
{
	GENERATED_BODY()
	
public:
	ADSNonCharacter(const FObjectInitializer& ObjectInitializer);

};
