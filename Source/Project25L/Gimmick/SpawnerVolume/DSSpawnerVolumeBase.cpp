
// Default
#include "Gimmick/SpawnerVolume/DSSpawnerVolumeBase.h"

// UE

// Game
#include "AI/NPC/DSNonCharacter.h"
#include "GameData/DSGameDataSubsystem.h"
#include "System/DSSpawnerSubsystem.h"

FVector ADSSpawnerVolumeBase::CalculateRandomPosition()
{
	FVector LocationPivot = GetActorLocation();

	float MaxX = LocationPivot.X + Brush->Bounds.BoxExtent.X;
	float MaxY = LocationPivot.Y + Brush->Bounds.BoxExtent.Y;

	float MinX = LocationPivot.X - Brush->Bounds.BoxExtent.X;
	float MinY = LocationPivot.Y - Brush->Bounds.BoxExtent.Y;


	float X = FMath::RandRange(MinX, MaxX); //int에 대해서 나오는 랜덤
	float Y = FMath::RandRange(MinY, MaxY);

	return FVector(X, Y, LocationPivot.Z);
}

void ADSSpawnerVolumeBase::SpawnMonster(EMonsterType MonsterType)
{
	UDSSpawnerSubsystem* SpawnSusbystem = UDSSpawnerSubsystem::Get(this);

	check(SpawnSusbystem);

	for (int i = 0; i < SpawnCount; i++)
	{
		FVector Location = CalculateRandomPosition();
		SpawnSusbystem->CreateMonster(MonsterType, Location);
	}
}

void ADSSpawnerVolumeBase::SpawnItem(TArray<int32>& ItemIDs)
{
	UDSSpawnerSubsystem* EnemySusbystem = UDSSpawnerSubsystem::Get(this);

	check(EnemySusbystem);

	//확률 계산한다.
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	for (int i = 0; i < SpawnCount; i++)
	{
		FVector Location = CalculateRandomPosition();
		EnemySusbystem->CreateItem(ItemIDs, Location);
	}
}
