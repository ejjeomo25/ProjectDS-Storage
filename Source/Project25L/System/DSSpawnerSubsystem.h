#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/WorldSubsystem.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSSpawnerSubsystem.generated.h"

class AActor;

USTRUCT()
struct FSpawnItemInfo
{
	GENERATED_BODY()

public:
	FSpawnItemInfo() {}
	FSpawnItemInfo(int32 ItemID, float Prob)
		: ItemID(ItemID)
		, SpawnProbability(Prob) {}

	int32 ItemID;
	float SpawnProbability;
};

UCLASS()
class PROJECT25L_API UDSSpawnerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UDSSpawnerSubsystem();

	static UDSSpawnerSubsystem* Get(UObject* Object);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	AActor* CreateMonster(EMonsterType MonsterType, FVector& Location);

	AActor* CreateItem(TArray<int32>& ItemIDs, FVector& Location);

protected:

	int32 GetRandomItemID(TArray<int32>& ItemIDs);

protected:

	UPROPERTY(Transient)
	TArray<FSpawnItemInfo> ItemInfo;


};
