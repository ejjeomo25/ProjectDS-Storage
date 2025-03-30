#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"
#include "System/DSEnums.h"

// Game
#include "Item/Weapon/DSWeapon.h"

// UHT
#include "DSWeaponData.generated.h"

USTRUCT(BlueprintType)
struct FDSWeaponData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSWeaponData() :
		EquipMontage(nullptr)
		, UnEquipMontage(nullptr)
		, Weapon(nullptr)
		, InputThreshold(0.f)
		, BulletCount(0)
		, BulletSpread(0.f)
		, SpreadCoef(0.f)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<UAnimMontage> UnEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftClassPtr<ADSWeapon> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float InputThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Gun")
	int32 BulletCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Gun")
	float BulletSpread;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Gun")
	float SpreadCoef;
};