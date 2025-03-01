#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Gimmick/SpawnerVolume/DSSpawnerVolumeBase.h"

// UHT
#include "DSMonsterSpawnerVolume.generated.h"

UCLASS()
class PROJECT25L_API ADSMonsterSpawnerVolume : public ADSSpawnerVolumeBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
protected:

	UPROPERTY(EditAnywhere, Category = MonsterType)
	EMonsterType MonsterType;

};
