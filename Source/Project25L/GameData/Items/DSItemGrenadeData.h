#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemGrenadeData.generated.h"

USTRUCT(BlueprintType)
struct FDSItemGrenadeData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FDSItemGrenadeData()
		: ThrowRange(0.f)
		, ThrowDamage(0.f)
		, AnimationOverride(nullptr)
	{}
	
	UPROPERTY(EditAnywhere, Category = Item)
	float ThrowRange;

	UPROPERTY(EditAnywhere, Category = Item)
	float ThrowDamage;

	UPROPERTY(EditAnywhere, Category = Item)
	TSoftObjectPtr<UAnimMontage> AnimationOverride;
};