#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemVehicleData.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FDSItemVehicleData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemVehicleData()
		: Duration(0.0f)
		, WalkSpeed(0.f)
		, RunSpeed(0.f)
		, JumpHeight(0.f)
		, bCanFly(0)
		, AnimationOverride(nullptr)
	{}
	
	UPROPERTY(EditAnywhere, Category = Item)
	float Duration;
	
	UPROPERTY(EditAnywhere, Category = Item)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = Item)
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = Item)
	float JumpHeight;

	UPROPERTY(EditAnywhere, Category = Item)
	bool bCanFly;

	UPROPERTY(EditAnywhere, Category = Item)
	TSoftObjectPtr<UAnimMontage> AnimationOverride;

};