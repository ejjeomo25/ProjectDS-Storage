#pragma once

// Defualt 
#include "CoreMinimal.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "System/DSEnums.h"
#include "Interface/DSAnimationAttackInterface.h"

// UHT
#include "DSCharacter_Girl.generated.h"

UCLASS()
class PROJECT25L_API ADSCharacter_Girl : public ADSArmedCharacter, public IDSAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer) ;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AttackHitCheck() override;
};
