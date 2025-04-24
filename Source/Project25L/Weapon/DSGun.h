
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

	virtual void AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance) const;
	virtual void AttackSkill1(const FVector& TargetLocation, const float& AttackRadius, const float& Impulse);
	virtual void AttackSkill2(const float& AutoAimAngle, const float& AttackDistance, const float& AttackRadius);

	FVector GetMuzzleSocketLocation() const;
	/*Gun의 공격*/
	virtual FVector GetFinalDestination(const float& AimAngle, const float& SkillDistance) const override;

	virtual void EndPrimaryAttack() const override;

	virtual void EndAttackSkill1() override;
protected:	
	/*무기를 가졌는데, 원거리 공격을 수행할 경우 탄 퍼짐 현상이 존재한다. 이때 사용하는 함수*/
	FVector GetSpreadShotTargetLocation(const FVector& TargetDir) const;
	
	void CancelSkill1();
protected:
	/*RPC*/
	UFUNCTION(Server, Reliable)
	void ServerRPC_SkillAttack(const FVector& TargetLoc, const float& SkillDistance, const float& Impulse);

	UFUNCTION(Server, Reliable)
	void ServerRPC_PrimaryAttack(const FVector& StartLoc, const FVector& TargetDir) const;

	UFUNCTION(Server, Reliable)
	void ServerRPC_EndAttackSkill1();

protected:
	virtual void InitializeData() override; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> GravityGunTargets;

	UPROPERTY(Transient, Replicated)
	float SpreadCoef;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> YawRecoil;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> PitchRecoil;

	UPROPERTY(Transient)
	mutable int32 ShotsFired;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Path")
	FString FilePath;
	
	UPROPERTY(EditAnywhere, Category = "DSSettings | Curve")
	TSoftObjectPtr<class UCurveFloat> YawRecoilClass;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Curve")
	TSoftObjectPtr<class UCurveFloat> PitchRecoilClass;

};
