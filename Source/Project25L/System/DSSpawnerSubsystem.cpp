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
	Super::OnWorldBeginPlay(InWorld);

	InitializeData();
}

void UDSSpawnerSubsystem::InitializeData()
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


TMap<int32, int32> UDSSpawnerSubsystem::SelectChestItems(TArray<int32>& ItemIDs, int32 MaxRange, int32 MinRange, FVector& Location, TWeakObjectPtr<AActor> ItemActor)
{
	UWorld* World = GetWorld();

	check(World);

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	int32 RandomCount = FMath::RandRange(MinRange, MaxRange);

	TMap<int32, int32> Result;
	int32 ItemID = 0;

	float TotalProbability = GetTotalItemProbability(ItemIDs);

	for (int32 RandomIdx = 0; RandomIdx < RandomCount; RandomCount++)
	{
		ItemID = GetRandomItemID(ItemIDs, TotalProbability);

		//아이템이 0번인 경우는 없음.
		if (ItemID == 0)
		{
			continue;
		}

		if (Result.Contains(ItemID) == false)
		{
			Result.Add({ ItemID, 1 });
		}
		else
		{
			Result[ItemID]++;
		}

		DS_LOG(DSItemLog, Log, TEXT("Selected Item %d, Spawn!"), ItemID);
	}

	if (Result.IsEmpty() == false)
	{
		//박스 중간으로 만든다.
		//보물상자 번호를 넣어준다.
		ItemActor = CreateActor(ESpawnerType::RangeItem, 101, Location);
	}
	return Result;
}

AActor* UDSSpawnerSubsystem::CreateActor(ESpawnerType SpawnType,int32 SpawnID, FVector& Location)
{
	/*
	몬스터랑 아이템 스포너 둘을 비슷하도록 만든다.
	현재 아이템 스포너 int32 아이디를 가지는 것처럼 몬스터 스포너 또한, int32를 사용해서 아이디를 가질 수 있도록 만든다.
	*/

	UWorld* World = GetWorld();

	check(World);

	//지정된 몬스터 위치에 몬스터를 만든다.
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);
	
	TSubclassOf<AActor> ActorClass = nullptr;

	if (SpawnType == ESpawnerType::RangeMonster)
	{
		FDSNonCharacterStat*NonCharacterData = static_cast<FDSNonCharacterStat*>(DataSubsystem->GetDataRow(EDataTableType::NonCharacterData, SpawnID));

		ActorClass = NonCharacterData->ActorClass;

	}
	else
	{
		FDSItemData* ItemData = static_cast<FDSItemData*>(DataSubsystem->GetDataRow(EDataTableType::ItemData, SpawnID));

		ActorClass = ItemData->ActorClass;
	}

	AActor* SpawnObj = nullptr;

	if (IsValid(ActorClass))
	{
		FActorSpawnParameters Params;

		SpawnObj = World->SpawnActor<AActor>(ActorClass, Location, FRotator::ZeroRotator, Params);

		return SpawnObj;
	}

	return nullptr;
}

int32 UDSSpawnerSubsystem::GetRandomItemID(TArray<int32>& ItemIDs, float TotalProbability)
{
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

float UDSSpawnerSubsystem::GetTotalItemProbability(TArray<int32>& ItemIDs)
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

	return TotalProbability;
}
