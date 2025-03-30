
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

	switch (SpawnerType)
	{
	case ESpawnerType::RangeItem:
		SpawnInFixedRange();
		break;
	case ESpawnerType::FixedItem:
		SpawnAtFixedLocation();
		break;
	case ESpawnerType::RangeMonster:
		break;
	default:
		break;
	}
}

void ADSSpawnerVolumeBase::SpawnInFixedRange()
{
	UDSSpawnerSubsystem* SpawnerSubsystem = UDSSpawnerSubsystem::Get(this);

	check(SpawnerSubsystem);

	//실제 아이템을 스폰한다.

	for (int SpawnID = 0; SpawnID < SpawnIDs.Num(); SpawnID++)
	{
		int32 SpawnThreshold = FMath::RandRange(SpawnMin, SpawnMax);

		for (int SpawnCount = 0; SpawnCount < SpawnThreshold; SpawnCount++)
		{
			FVector Location = CalculateRandomPosition();
			TWeakObjectPtr<AActor> Actor = SpawnerSubsystem->CreateActor(SpawnerType, SpawnIDs[SpawnID], Location);

			if (Actor->IsValidLowLevel())
			{
				ADSItemActor* ItemActor = Cast<ADSItemActor>(Actor);

				if (IsValid(ItemActor))
				{
					TMap<int32, int32> StoredItem;
					StoredItem.Add({ ItemActor->GetID(), 1 });

					ItemActor->InitializeItemData(StoredItem);
				}
			}
		}
	}
}

void ADSSpawnerVolumeBase::SpawnAtFixedLocation()
{
	UDSSpawnerSubsystem* SpawnerSubsystem = UDSSpawnerSubsystem::Get(this);

	check(SpawnerSubsystem);

	//정규 분포식을 사용해서 아이템을 스폰하지 않고 어떤 아이템이 들어 있는지에 대한 답만 가져온다.
	//박스 메쉬를 중앙 위치에 생성한다.
	FVector LocationPivot = GetActorLocation();

	FVector Location = { LocationPivot.X + (Brush->Bounds.BoxExtent.X / 2) , LocationPivot.Y + (Brush->Bounds.BoxExtent.Y / 2), LocationPivot.Z };

	TMap<int32, int32> StoredItem = SpawnerSubsystem->SelectChestItems(SpawnIDs, SpawnMax, SpawnMin, Location);
	TWeakObjectPtr<ADSItemActor> TreasureBox = Cast<ADSItemActor>(SpawnerSubsystem->CreateActor(ESpawnerType::RangeItem, SpawnGiftID, Location));

	//StoredItem를 박스 메쉬에게 전달한다.
	if (TreasureBox.IsValid())
	{
		TreasureBox->InitializeItemData(StoredItem);
	}

}
