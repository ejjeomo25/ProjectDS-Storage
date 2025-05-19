#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSArmedCharacter.h"

// UHT
#include "DSCharacter_Boy.generated.h"

UCLASS()
class PROJECT25L_API ADSCharacter_Boy : public ADSArmedCharacter
{
	GENERATED_BODY()
	
public:
	ADSCharacter_Boy(const FObjectInitializer& ObjectInitializer);

};
