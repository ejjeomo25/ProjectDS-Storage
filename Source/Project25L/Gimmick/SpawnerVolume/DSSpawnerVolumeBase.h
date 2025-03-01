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
	virtual FVector CalculateRandomPosition();
	
protected:
	
	void SpawnMonster(EMonsterType MonsterType);
	void SpawnItem(TArray<int32>& ItemIDs);
protected:
	
	UPROPERTY(EditAnywhere, Category = Count)
	int32 SpawnCount;
};
