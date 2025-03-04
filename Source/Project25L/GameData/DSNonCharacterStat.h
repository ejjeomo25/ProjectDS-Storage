#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSNonCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FDSNonCharacterStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSNonCharacterStat() :
		MaxHp(0.0f),
		BaseDamage(0.0f),
		BaseDefense(0.0f) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseDefense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Class)
	TSubclassOf<AActor> ActorClass;

};