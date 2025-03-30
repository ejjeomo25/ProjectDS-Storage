#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSWeapon.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

UCLASS()
class PROJECT25L_API ADSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSWeapon();

	float GetInputThreshold() { return InputThreshold; }

	/* 클래스를 상속받았을 경우 반드시 구현해주어야하는 함수입니다. */
	virtual void PrimaryAttack(const float& AimAngle, const float& SkillRadius) const;
protected:
	/*auto targeting*/
	//무기를 가진 모든 캐릭터는 오토 타겟팅이 가능하기 때문에 Weapon함수에 존재.
	FVector GetAutoTargetingLocation(const float& AimAngle, const float& AttackRadius) const;
	/*무기를 가졌는데, 원거리 공격을 수행할 경우 탄 퍼짐 현상이 존재한다. 이때 사용하는 함수*/

	FVector GetSpreadShotTargetLocation(const FVector& TargetDir, float SpreadCoef) const;

	virtual void InitializeData();
protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(Transient, Replicated)
	float InputThreshold;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(Transient)
	TObjectPtr< UAnimMontage> UnEquipMontage;

	UPROPERTY(EditAnywhere, Category = "Setting | Type")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Setting | Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh;

};
