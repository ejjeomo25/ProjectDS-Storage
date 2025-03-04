
// Default
#include "Gimmick/SpawnerVolume/DSSpawnerVolumeBase.h"

// UE

// Game
#include "Item/DSItemActor.h"
#include "GameData/DSGameDataSubsystem.h"
#include "System/DSSpawnerSubsystem.h"



ADSSpawnerVolumeBase::ADSSpawnerVolumeBase()
{
}

void ADSSpawnerVolumeBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnActors();
	}
}

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

void ADSSpawnerVolumeBase::SpawnActors()
{
	UDSSpawnerSubsystem* SpawnerSubsystem = UDSSpawnerSubsystem::Get(this);

	check(SpawnerSubsystem);

	//확률 계산한다.
	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	if (SpawnerType == ESpawnerType::FixedItem)
	{
		//정규 분포식을 사용해서 아이템을 스폰하지 않고 어떤 아이템이 들어 있는지에 대한 답만 가져온다.
		//박스 메쉬를 중앙 위치에 생성한다.

		TWeakObjectPtr<ADSItemActor> TreasureBox = nullptr;
		
		FVector LocationPivot = GetActorLocation();

		FVector Location = { LocationPivot.X + (Brush->Bounds.BoxExtent.X / 2) , LocationPivot.Y + (Brush->Bounds.BoxExtent.Y / 2), LocationPivot.Z };

		TMap<int32, int32> StoredItem = SpawnerSubsystem->SelectChestItems(SpawnIDs, SpawnMax, SpawnMin, Location, TreasureBox);
		
		if (TreasureBox->IsValidLowLevel())
		{
			TreasureBox->InitializeItemData(StoredItem);
		}
		//StoredItem를 박스 메쉬에게 전달한다.
	}
	else
	{
		//실제 아이템을 스폰한다.

		for (int SpawnID = 0; SpawnID < SpawnIDs.Num(); SpawnID)
		{
			int32 SpawnCount = FMath::RandRange(SpawnMin, SpawnMax);

			for (int i = 0; i < SpawnCount; i++)
			{
				FVector Location = CalculateRandomPosition();
				SpawnerSubsystem->CreateActor(SpawnerType, SpawnID, Location);
			}
		}
	}
}