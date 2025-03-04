#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "Engine/TriggerVolume.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSSpawnerVolumeBase.generated.h"


UCLASS()
class PROJECT25L_API ADSSpawnerVolumeBase : public ATriggerVolume
{
	GENERATED_BODY()
	
public:

	ADSSpawnerVolumeBase();

protected:

	virtual FVector CalculateRandomPosition();

	virtual void BeginPlay() override;

	void SpawnActors();

protected:
	UPROPERTY(EditAnywhere, Category = Count)
	int32 SpawnMax;

	UPROPERTY(EditAnywhere, Category = Count)
	int32 SpawnMin;

	UPROPERTY(EditAnywhere, Category = ItemSpawnerType)
	ESpawnerType SpawnerType;

	UPROPERTY(EditAnywhere, Category = MonsterType)
	TArray<int32> SpawnIDs;

};
