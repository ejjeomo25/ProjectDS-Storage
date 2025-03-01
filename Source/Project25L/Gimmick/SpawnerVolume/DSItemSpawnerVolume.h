// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/SpawnerVolume/DSSpawnerVolumeBase.h"
#include "DSItemSpawnerVolume.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT25L_API ADSItemSpawnerVolume : public ADSSpawnerVolumeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
protected:

	UPROPERTY(EditAnywhere, Category = MonsterType)
	TArray<int32> ItemIDs;
};
