// Default
#include "UI/Inventory/DSInventoryGridWidget.h"

<<<<<<< HEAD
// UE
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"

// Game
#include "Components/DSInventoryComponent.h"
#include "UI/Inventory/DSInventoryItem.h"
#include "System/DSEventSystems.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "System/DSGameDataSubsystem.h"
#include "DSLogChannels.h"
#include "System/DSSpawnerSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UDSInventoryGridWidget::UDSInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, Inventory(nullptr)
	, TileSize(0)
{
}

bool UDSInventoryGridWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	const auto& MouseKey = InDragDropEvent.GetPressedButtons();
	if (MouseKey.Contains(EKeys::RightMouseButton))
	{
		return bSuperHandled;
	}
	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return bSuperHandled;
	}

	int TopLeftIndex = Inventory->TiletoIndex(ItemDragDrop->TopLeftIndex, Inventory->GetPersonalInventoryInfo().Columns);
	if (Inventory->IsRoomAvailable(ItemDragDrop->ItemInfo, TopLeftIndex))
	{
		Inventory->AddItemAt(ItemDragDrop->ItemInfo, TopLeftIndex);
	}
	//빈 자리 있으면 넣고 아니면 생성
	else if(false == Inventory->StoreItems(nullptr, ItemDragDrop->ItemInfo))
	{
		UDSSpawnerSubsystem* SpawnManager = UDSSpawnerSubsystem::Get(this);
		if (false == IsValid(SpawnManager))
		{
			return false;
		}

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (false == IsValid(PC))
		{
			return false;
		}
		APawn* PlayerPawn = PC->GetPawn();
		if (false == IsValid(PlayerPawn))
		{
			return false;
		}

		FVector2D Offset2D = FMath::RandPointInCircle(150.f);
		FVector RandomLocation = PlayerPawn->GetActorLocation() + FVector(Offset2D.X, Offset2D.Y, 0.f);
		SpawnManager->CreateActor(ESpawnerType::FixedItem, ItemDragDrop->ItemInfo.ID, RandomLocation);
	}
	
	return true;
}

bool UDSInventoryGridWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	FVector2D ScreenPosition = InDragDropEvent.GetScreenSpacePosition();
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(ScreenPosition);

	UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return bSuperHandled;
	}

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemDragDrop->ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return bSuperHandled;
	}


	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemDragDrop->ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;


	int32 TileX = FMath::TruncToInt(LocalPosition.X / TileSize);
	int32 TileY = FMath::TruncToInt(LocalPosition.Y / TileSize);

	FIntPoint Tile(TileX, TileY);

	FIntPoint Direction = MousePositionInIntile(LocalPosition, bIsRotated);

	int32 OffsetX = (FinalSize.X % 2 == 0) ? (1 - Direction.X) : FinalSize.X / 2;
	int32 OffsetY = (FinalSize.Y % 2 == 0) ? (1 - Direction.Y) : FinalSize.Y / 2;

	int32 TopLeftX = Tile.X - OffsetX;
	int32 TopLeftY = Tile.Y - OffsetY;

	int32 MaxColumns = Inventory->GetPersonalInventoryInfo().Columns;
	int32 MaxRows = Inventory->GetPersonalInventoryInfo().Rows;

	TopLeftX = FMath::Clamp(TopLeftX, 0, MaxColumns - FinalSize.X);
	TopLeftY = FMath::Clamp(TopLeftY, 0, MaxRows - FinalSize.Y);

	ItemDragDrop->TopLeftIndex = FIntPoint(TopLeftX, TopLeftY);
	// DS_LOG(DSUILog, Log, TEXT("TopLeftIndex : X : %d, Y : %d"), TopLeftX, TopLeftY);

	return true;

}



FReply UDSInventoryGridWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::R)
	{
		UDragDropOperation* DragOp = UWidgetBlueprintLibrary::GetDragDroppingContent();
		UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(DragOp);
		if (false == IsValid(ItemDragDrop))
		{
			return FReply::Unhandled();
		}
		UDSInventoryItem* DragVisual = Cast<UDSInventoryItem>(ItemDragDrop->DefaultDragVisual);
		if (false == IsValid(DragVisual))
		{
			return FReply::Unhandled();
		}

		// 회전 처리
		ItemDragDrop->ItemInfo.isRotated = !ItemDragDrop->ItemInfo.isRotated;

		// DragVisual 갱신
		DragVisual->InitializeItem(ItemDragDrop->ItemInfo, TileSize, ItemDragDrop->TopLeftIndex);
		DragVisual->SetRenderOpacity(0.5f); // 투명도 유지
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}


void UDSInventoryGridWidget::Init(UDSInventoryComponent* InventoryComponent, int InTileSize)
{
	Inventory = InventoryComponent;
	TileSize = InTileSize;

	DSEVENT_DELEGATE_BIND(Inventory->OnInventoryChanged, this, &UDSInventoryGridWidget::Refresh);
	if (IsValid(GridBorder))
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
		if (IsValid(CanvasSlot))
		{
			CanvasSlot->SetSize(FVector2D(InventoryComponent->GetPersonalInventoryInfo().Columns * TileSize, InventoryComponent->GetPersonalInventoryInfo().Rows * TileSize));

			CreateLineSegments();
		}

	}
}

void UDSInventoryGridWidget::CreateLineSegments()
{
	for (int X = 0; X <= Inventory->GetPersonalInventoryInfo().Columns; ++X)
	{
		int XPos = X * TileSize;

		FDSLine Line(XPos, 0, XPos, Inventory->GetPersonalInventoryInfo().Rows * TileSize);
		Lines.Add(Line);
	}

	for (int Y = 0; Y <= Inventory->GetPersonalInventoryInfo().Rows; ++Y)
	{
		int YPos = Y * TileSize;

		FDSLine Line(0, YPos, Inventory->GetPersonalInventoryInfo().Columns * TileSize, YPos);
		Lines.Add(Line);
	}

	Refresh();
}

void UDSInventoryGridWidget::Refresh()
{
	
	if (IsValid(GridCanvasPanel))
	{
		GridCanvasPanel->ClearChildren();

		TMap<FIntPoint, FDSItemInfo> AllItems = Inventory->GetAllItems();

		for (const TPair<FIntPoint, FDSItemInfo>& ItemPair : AllItems)
		{
			FIntPoint TopLeftIndex = ItemPair.Key;
			const FDSItemInfo& ItemInfo = ItemPair.Value;

			APlayerController* OwningPlayer = GetOwningPlayer();
			if (false == IsValid(OwningPlayer))
			{
				return ;
			}

			UDSInventoryItem* ItemWidget = CreateWidget<UDSInventoryItem>(OwningPlayer, InventoryItemClass);
			if (IsValid(ItemWidget))
			{
				ItemWidget->InitializeItem(ItemInfo, TileSize, TopLeftIndex);
				DSEVENT_DELEGATE_BIND(ItemWidget->OnItemDragged, Inventory, &UDSInventoryComponent::RemoveItem);
			}

			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(ItemWidget));
			if (IsValid(CanvasSlot))
			{
				CanvasSlot->SetAutoSize(true);
				CanvasSlot->SetPosition(FVector2D(TopLeftIndex.X * TileSize, TopLeftIndex.Y * TileSize));
			}
		}
	}
}

FIntPoint UDSInventoryGridWidget::MousePositionInIntile(FVector2D MousePosition, bool bIsRotated)
{
	// 00 : 왼쪽 위
	// 10 : 오늘쪽 위
	// 01 : 왼쪽 아래
	// 11 : 오늘쪽 아래
	const float HalfTile = TileSize / 2.f;

	const float ModX = FMath::Fmod(MousePosition.X, TileSize);
	const float ModY = FMath::Fmod(MousePosition.Y, TileSize);

	int32 Right = (ModX > HalfTile) ? 1 : 0;
	int32 Down = (ModY > HalfTile) ? 1 : 0;

	if (bIsRotated)
	{
		return FIntPoint(Down, Right);
	}
	return FIntPoint(Right, Down);
	
}


=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
