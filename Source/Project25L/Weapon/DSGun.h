
#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Weapon/DSWeapon.h"

// UHT
#include "DSGun.generated.h"

class UTimelineComponent;

UCLASS()
class PROJECT25L_API ADSGun : public ADSWeapon
{
	GENERATED_BODY()
	
public:
	ADSGun();
	
protected:
	/*Gun의 공격*/
	virtual void PrimaryAttack(const float& AimAngle, const float& SkillRadius) const override;

	virtual void EndPrimaryAttack() override;
protected:	
	UFUNCTION(Server, Reliable)
	void ServerRPC_PrimaryAttack(const FVector& StartLoc, const FVector& TargetDir) const;

	/*무기를 가졌는데, 원거리 공격을 수행할 경우 탄 퍼짐 현상이 존재한다. 이때 사용하는 함수*/
	FVector GetSpreadShotTargetLocation(const FVector& TargetDir) const;

protected:
	virtual void InitializeData() override; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

protected:

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> YawRecoil;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> PitchRecoil;

	UPROPERTY(Transient, Replicated)
	float SpreadCoef;

	UPROPERTY(EditAnywhere, Category = "Setting | Curve")
	mutable float ShotsFired;

	UPROPERTY(EditAnywhere, Category = "Setting | Curve")
	TSoftObjectPtr<class UCurveFloat> YawRecoilClass;

	UPROPERTY(EditAnywhere, Category = "Setting | Curve")
	TSoftObjectPtr<class UCurveFloat> PitchRecoilClass;

	UPROPERTY(EditAnywhere, Category = "Setting | Path")
	FString FilePath;

};
