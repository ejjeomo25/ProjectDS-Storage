#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemData.generated.h"


USTRUCT(BlueprintType)
struct FDSItemData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemData()
		: ItemName(TEXT(""))
		, ItemHolder(0x00)
		, Probability(0.0f)
		, ActorClass(nullptr)
		, Description(TEXT(""))
	{}

	UPROPERTY(EditAnywhere, Category = Name)
	FString ItemName;
	
	UPROPERTY(EditAnywhere, Category = Holder)
	uint8 ItemHolder;

	UPROPERTY(EditAnywhere, Category = Prop)
	float Probability;

	UPROPERTY(EditAnywhere, Category = Actor)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = Desc)
	FString Description;
	
};