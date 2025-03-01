#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemAccessoryData.generated.h"

USTRUCT(BlueprintType)
struct FDSItemAccessoryData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemAccessoryData()
		: Duration(0.f)
		, AnimationOverride(nullptr)
	{}
		
	UPROPERTY(EditAnywhere, Category = Item)
	float Duration;

	UPROPERTY(EditAnywhere, Category = Item)
	TSoftObjectPtr<UAnimMontage> AnimationOverride;

};