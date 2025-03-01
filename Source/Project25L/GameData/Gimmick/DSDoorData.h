#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataAsset.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSDoorData.generated.h"

class UMaterialInterface;

UCLASS()
class PROJECT25L_API UDSDoorData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Change)
	EDungeonType DungeonType;

	UPROPERTY(EditAnywhere, Category = Change)
	TObjectPtr<UMaterialInterface> ChangedMateral;

	UPROPERTY(EditAnywhere, Category = Change)
	TObjectPtr<UMaterialInterface> ChangedBeadMateral;

	UPROPERTY(EditAnywhere, Category = Change)
	FString Destination;
};
