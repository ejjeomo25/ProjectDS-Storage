
#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Item/Weapon/DSWeapon.h"

// UHT
#include "DSGun.generated.h"

UCLASS()
class PROJECT25L_API ADSGun : public ADSWeapon
{
	GENERATED_BODY()
	
public:
	ADSGun();

protected:

	virtual void InitializeData() override; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(Transient, Replicated)
	int32 BulletCount;
	
	UPROPERTY(Transient, Replicated)
	float BulletSpread;
	
	UPROPERTY(Transient, Replicated)
	float SpreadCoef;
};
