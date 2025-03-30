
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
	virtual void PrimaryAttack(const float& AimAngle, const float& SkillRadius) const override;
	virtual void InitializeData() override; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
protected:
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_PrimaryAttack() const;

protected:

	UPROPERTY(Transient, Replicated)
	float BulletSpread;
	
	UPROPERTY(Transient, Replicated)
	float SpreadCoef;

	UPROPERTY(EditAnywhere, Category = "Setting | SpreadFile")
	FString FilePath = "SpreadOffsetFile.txt";

};
