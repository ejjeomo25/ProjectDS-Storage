// Default
#include "System/DSSpawnerSubsystem.h"

// UE

// Game
#include "AI/NPC/DSNonCharacter.h"
#include "GameData/DSGameDataSubsystem.h"
#include "GameData/DSNonCharacterStat.h"
#include "GameData/Items/DSItemData.h"

#include "DSLogChannels.h"

UDSSpawnerSubsystem::UDSSpawnerSubsystem()
	: Super()
	, ItemInfo()
{
}

UDSSpawnerSubsystem* UDSSpawnerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	return World->GetSubsystem<UDSSpawnerSubsystem>();
}

void UDSSpawnerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	UDataTable* ItemDataTable = DataSubsystem->GetDataTable(EDataTableType::ItemData);

	if (IsValid(ItemDataTable) == false)
	{
		return;
	}

	TMap<FName, uint8*> RowMap = ItemDataTable->GetRowMap();

	// SpawnerSubsystem에서 필요로 하는 필수 종류만 구조체로 가지고 있는다.
	for (const auto& Row : RowMap)
	{
		FDSItemData* ItemData = reinterpret_cast<FDSItemData*>(Row.Value);
		if (nullptr != ItemData)
		{
			int ItemID = FCString::Atoi(*(Row.Key.ToString()));

			ItemInfo.Add({ ItemID, ItemData->Probability });
		}
	}
}


AActor* UDSSpawnerSubsystem::CreateMonster(EMonsterType MonsterType, FVector& Location)
{
	UWorld* World = GetWorld();

	check(World);

	//지정된 몬스터 위치에 몬스터를 만든다.
	UDSGameDataSubsystem *DataSubsystem= UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	FDSNonCharacterStat *NonCharacterData= DataSubsystem->GetDataRow<FDSNonCharacterStat, EMonsterType>(EDataTableType::NonCharacterData, MonsterType);
	AActor* Monster = nullptr;

	if (nullptr != NonCharacterData)
	{
		FActorSpawnParameters Params;

		Monster = World->SpawnActor<AActor>(NonCharacterData->ActorClass, Location, FRotator::ZeroRotator, Params);
	}
	
	return Monster;
}

AActor* UDSSpawnerSubsystem::CreateItem(TArray<int32>& ItemIDs, FVector& Location)
{
	UWorld* World = GetWorld();

	check(World);

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	int32 ItemID = GetRandomItemID(ItemIDs);

	//아이템이 0번인 경우는 없음.
	if (ItemID == 0)
	{
		return nullptr;
	}

	DS_LOG(DSItemLog, Log, TEXT("Selected Item %d, Spawn!"), ItemID);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataSubsystem->GetItemData(EDataTableType::ItemData, ItemID));
	AActor* Item = nullptr;

	if (nullptr != ItemData)
	{
		FActorSpawnParameters Params;

		Item = World->SpawnActor<AActor>(ItemData->Blueprint, Location, FRotator::ZeroRotator, Params);
	}

	return Item;
}

int32 UDSSpawnerSubsystem::GetRandomItemID(TArray<int32>& ItemIDs)
{

	float TotalProbability = 0.0f;

	// 정규분포를 위해서 ID가 있을 경우에 더한다.
	for (int ItemIdx = 0; ItemIdx < ItemIDs.Num(); ItemIdx++)
	{
		for (int ArrayIdx = 0; ArrayIdx < ItemInfo.Num(); ArrayIdx++)
		{
			if (ItemIDs[ItemIdx] == ItemInfo[ArrayIdx].ItemID)
			{
				TotalProbability += ItemInfo[ArrayIdx].SpawnProbability;
				break;
			}
		}
	}

	// 랜덤으로 뽑아서,
	float RandomValue = FMath::FRandRange(0.0f, TotalProbability);

	// 누적 값이 RandomValue보다 높아질 경우, ID를 반환한다.
	float AccumulatedProbability = 0.0f;

	for (const FSpawnItemInfo& Item : ItemInfo)
	{
		AccumulatedProbability += Item.SpawnProbability;
		if (RandomValue <= AccumulatedProbability)
		{
			return Item.ItemID;
		}
	}

	return 0;
}
