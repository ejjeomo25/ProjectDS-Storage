#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "GameData/DSEnums.h"

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

	virtual void EndPrimaryAttack();
protected:
	/*auto targeting*/
	//무기를 가진 모든 캐릭터는 오토 타겟팅이 가능하기 때문에 Weapon함수에 존재.
	FVector GetAutoTargetingLocation(const float& AimAngle, const float& AttackRadius) const;

	virtual void InitializeData();
protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(Transient, Replicated)
	float InputThreshold;

	UPROPERTY(EditAnywhere, Category = "Setting | Type")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Setting | Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, Category = "Setting | Weapon")
	uint8 bUseAutoTargeting : 1;

};
