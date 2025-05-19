// Default
#include "UI/Inventory/DSInventoryWidget.h"

<<<<<<< HEAD
// UE
#include "Kismet/GameplayStatics.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSInventoryComponent.h"
#include "system/DSGameUtils.h"
#include "system/DSSpawnerSubsystem.h"
#include "UI/Inventory/DSInventoryGridWidget.h"
#include "DSItemDragDropOperation.h"
#include "Item/DSItemActor.h"


void UDSInventoryWidget::NativeConstruct()
{

	Super::NativeConstruct();

	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(GetOwningPlayer()));
	UDSInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (false == IsValid(Inventory))
	{
		return;
	}

	float TileSize = Inventory->GetPersonalInventoryInfo().TileSize;

	if (IsValid(InventoryGirdWidget))
	{
		InventoryGirdWidget->Init(Inventory, TileSize);
	}
}

bool UDSInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return bSuperHandled;
	}

	UDSSpawnerSubsystem* SpawnManager = UDSSpawnerSubsystem::Get(this);
	if (false == IsValid(SpawnManager))
	{
		return bSuperHandled;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (false == IsValid(PC))
	{
		return bSuperHandled;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (false == IsValid(PlayerPawn))
	{
		return bSuperHandled;
	}

	FVector2D Offset2D = FMath::RandPointInCircle(150.f);
	FVector RandomLocation = PlayerPawn->GetActorLocation() + FVector(Offset2D.X, Offset2D.Y, 0.f);
	TWeakObjectPtr<AActor> Actor = SpawnManager->CreateActor(ESpawnerType::FixedItem, ItemDragDrop->ItemInfo.ID, RandomLocation);
	if (Actor.IsValid())
	{
		ADSItemActor* ItemActor = Cast<ADSItemActor>(Actor);

		if (IsValid(ItemActor))
		{
			TMap<int32, int32> StoredItem;
			StoredItem.Add({ ItemDragDrop->ItemInfo.ID, 1 });
			ItemActor->InitializeItemData(StoredItem);
		}
	}
	return true;
}

=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
