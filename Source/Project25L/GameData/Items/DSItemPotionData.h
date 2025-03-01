#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemPotionData.generated.h"


USTRUCT(BlueprintType)
struct FDSItemPotionData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemPotionData()
		: Duration(0.0) 
	{}

	UPROPERTY(EditAnywhere, Category = Item)
	float Duration;
	
};