// Default
#include "Gimmick/DSDungeonDoor.h"

// UE
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"  // TActorIterator
#include "GeometryCollection/GeometryCollectionComponent.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Gimmick/DSDoorData.h"
#include "Gimmick/DSDungeonMat.h"
#include "System/DSEventSystems.h"
#include "System/DSRoomSubsystem.h"

ADSDungeonDoor::ADSDungeonDoor()
	: Super()
	, bIsUnlocked(false)
	, MatVolume(nullptr)
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	DungeonBead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeadMesh"));
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	RootComponent = Trigger;
	GeometryCollection->SetupAttachment(Trigger);
	DungeonBead->SetupAttachment(Trigger);

	if (IsValid(DungeonBead))
	{
		DungeonBead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	DSEVENT_DELEGATE_BIND(GameEvent.OnUnlockStage, this, &ADSDungeonDoor::UnlockDoor);
	DSEVENT_DELEGATE_BIND(GameEvent.OnDestroyStage, this, &ADSDungeonDoor::DestroyDoor);

}

void ADSDungeonDoor::UnlockDoor(EDungeonType CurrentType)
{
	if (IsValid(DungeonDoorData))
	{
		if (DungeonDoorData->DungeonType != CurrentType)
		{
			return;
		}

		bIsUnlocked = true;
	}

}

void ADSDungeonDoor::DestroyDoor(EDungeonType CurrentType)
{
	if (IsValid(DungeonDoorData) == false)
	{
		return;
	}

	if (IsValid(DungeonBead) == false)
	{
		return;
	}

	if (IsValid(GeometryCollection) == false)
	{
		return;
	}

	if (DungeonDoorData->DungeonType >= CurrentType)
	{
		return;
	}

	DungeonBead->SetMaterial(0, DungeonDoorData->ChangedBeadMateral);
	//구슬이 등장
	DungeonBead->SetVisibility(true);

	GeometryCollection->SetMaterial(0, DungeonDoorData->ChangedMateral);
	//Chaos Destruction 표현
	GeometryCollection->EnableClustering = false;

	if (IsValid(MatVolume))
	{
		MatVolume->UnbindEvents();
	}

}

void ADSDungeonDoor::InitializeStage()
{
	if (IsValid(DungeonDoorData) == false)
	{
		return;
	}

	if (IsValid(GeometryCollection) == false)
	{
		return;
	}

	UWorld* World = GetWorld();

	check(World);

	// World를 사용해서 Iterator 돌려서 Actor가져올 수 있도록 변경하기
	for (TActorIterator<ADSDungeonMat> It(World, ADSDungeonMat::StaticClass()); It; ++It)
	{
		ADSDungeonMat* DungeonMat = *It;

		if (IsValid(DungeonMat))
		{
			if (DungeonMat->GetDungeonDoorType() == DungeonDoorData->DungeonType)
			{
				MatVolume = DungeonMat;
				break;
			}
		}
	}


	UDSRoomSubsystem* RoomSubsystem = UDSRoomSubsystem::Get(this);

	check(RoomSubsystem);

	RoomSubsystem->InitializeStage();

	if (bIsUnlocked)
	{
		if (IsValid(MatVolume))
		{
			MatVolume->BindEvents();
		}
	
		// 색상 변경을 진행한다.
		GeometryCollection->SetMaterial(0, DungeonDoorData->ChangedMateral);
	}

}

void ADSDungeonDoor::BeginPlay()
{
	Super::BeginPlay();

	InitializeStage();
}

void ADSDungeonDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DSEVENT_DELEGATE_REMOVE(GameEvent.OnUnlockStage, this);
	DSEVENT_DELEGATE_REMOVE(GameEvent.OnDestroyStage, this);
}
