#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSCharacter.h"
#include "System/DSEnums.h"

// UHT
#include "DSArmedCharacter.generated.h"

class ADSWeapon;

UCLASS()
class PROJECT25L_API ADSArmedCharacter : public ADSCharacter
{
	GENERATED_BODY()
	
public:
	ADSArmedCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

protected:
	void LoadWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	
	UPROPERTY(Transient, Replicated)
	TObjectPtr<ADSWeapon> Weapon;

	UPROPERTY(EditAnywhere, Category = Weapon)
	EWeaponType WeaponType;

};
