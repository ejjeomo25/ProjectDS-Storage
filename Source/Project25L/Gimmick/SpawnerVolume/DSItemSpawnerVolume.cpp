// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/SpawnerVolume/DSItemSpawnerVolume.h"
#include "DSItemSpawnerVolume.h"

void ADSItemSpawnerVolume::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnItem(ItemIDs);
	}
}
