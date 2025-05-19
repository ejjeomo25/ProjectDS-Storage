// Default
#include "UI/Inventory/DSInventoryGridBase.h"

// UE
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

// Game
#include "Components/DSInventoryComponent.h"
#include "DSLogChannels.h"
#include "Item/DSItem.h"
#include "System/DSEventSystems.h"
#include "System/DSGameDataSubsystem.h"
#include "System/DSSpawnerSubsystem.h"
#include "UI/Inventory/DSInventoryItem.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "UI/Inventory/Widget/DSCombatInventoryWidget.h"



UDSInventoryGridBase::UDSInventoryGridBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, InventoryComponent(nullptr)
	, TileSize(0)
{
}

bool UDSInventoryGridBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bSuperHandled = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	const UDSItemDragDropOperation* ItemDragDrop = Cast<UDSItemDragDropOperation>(InOperation);
	if (false == IsValid(ItemDragDrop))
	{
		return false;
	}


	// 그 위치에 드래그 놓기
	int TopLeftIndex = InventoryComponent->TiletoIndex(ItemDragDrop->TopLeftIndex, InventoryInfo->Columns);
	if (InventoryComponent->IsRoomAvailable(ItemDragDrop->ItemInfo, TopLeftIndex))
	{
		InventoryComponent->AddItemAtFromGrid(ItemDragDrop->ItemInfo, TopLeftIndex);
		return true;
	}


	return true;
}

bool UDSInventoryGridBase::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

	const FIntPoint Direction = MousePositionInIntile(LocalPosition, bIsRotated);

	int32 OffsetX = (FinalSize.X % 2 == 0) ? (1 - Direction.X) : FinalSize.X / 2;
	int32 OffsetY = (FinalSize.Y % 2 == 0) ? (1 - Direction.Y) : FinalSize.Y / 2;

	int32 TopLeftX = Tile.X - OffsetX;
	int32 TopLeftY = Tile.Y - OffsetY;

	const int32 MaxColumns = InventoryInfo->Columns;
	const int32 MaxRows = InventoryInfo->Rows;

	TopLeftX = FMath::Clamp(TopLeftX, 0, MaxColumns - FinalSize.X);
	TopLeftY = FMath::Clamp(TopLeftY, 0, MaxRows - FinalSize.Y);

	ItemDragDrop->TopLeftIndex = FIntPoint(TopLeftX, TopLeftY);

	return true;
}

FReply UDSInventoryGridBase::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

		// 뚯쟾 泥섎━
		ItemDragDrop->ItemInfo.isRotated = !ItemDragDrop->ItemInfo.isRotated;

		// DragVisual 媛깆떊
		DragVisual->InitializeItem(InventoryCategory, ItemDragDrop->ItemInfo, TileSize, ItemDragDrop->TopLeftIndex);
		DragVisual->SetRenderOpacity(0.5f); // щ챸좎
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UDSInventoryGridBase::Init(EInventoryCategory InInventoryCategory, FPersonalInventoryInfo* InInventoryInfo, TArray<FDSItemInfo>* InPersonalInventory, UDSInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	InventoryInfo = InInventoryInfo;
	TileSize = InventoryInfo->TileSize;
	PersonalInventory = InPersonalInventory;
	InventoryCategory = InInventoryCategory;

	switch (InventoryCategory)
	{
	case EInventoryCategory::PersonalVehicle:
		DSEVENT_DELEGATE_BIND(InventoryComponent->OnVehicleInventorySlotUpdated, this, &UDSInventoryGridBase::Refresh);
		break;

	case EInventoryCategory::PersonalItem:
		DSEVENT_DELEGATE_BIND(InventoryComponent->OnItemInventorySlotUpdated, this, &UDSInventoryGridBase::Refresh);
		break;
	default:
		break;
	}

	if (IsValid(GridBorder))
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
		if (IsValid(CanvasSlot))
		{
			CanvasSlot->SetSize(FVector2D(InventoryInfo->Columns * TileSize, InventoryInfo->Rows * TileSize));

			CreateLineSegments();
		}

	}

}

void UDSInventoryGridBase::CreateLineSegments()
{
	Lines.Empty();
	for (int X = 0; X <= InventoryInfo->Columns; ++X)
	{
		int XPos = X * TileSize;

		FDSLine Line(XPos, 0, XPos, InventoryInfo->Rows * TileSize);
		Lines.Add(Line);
	}

	for (int Y = 0; Y <= InventoryInfo->Rows; ++Y)
	{
		int YPos = Y * TileSize;

		FDSLine Line(0, YPos, InventoryInfo->Columns * TileSize, YPos);
		Lines.Add(Line);
	}
}

void UDSInventoryGridBase::Refresh(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRemoved)
{
	if (bRemoved)
	{
		RemoveItemWidgetFromSlot(TopLeftIndex);
	}
	else
	{
		AddItemWidgetToSlot(ItemInfo, TopLeftIndex);
	}
}

void UDSInventoryGridBase::AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (false == IsValid(OwningPlayer))
	{
		return;
	}

	UDSInventoryItemBase* ItemWidget = CreateWidget<UDSInventoryItemBase>(OwningPlayer, InventoryItemClass);
	if (IsValid(ItemWidget))
	{
		ItemWidget->InitializeItem(InventoryCategory, ItemInfo, TileSize, TopLeftIndex);
		DSEVENT_DELEGATE_BIND(ItemWidget->OnItemDragged, InventoryComponent, &UDSInventoryComponent::RemoveItem);
	}

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(ItemWidget));
	if (IsValid(CanvasSlot))
	{
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetPosition(FVector2D(TopLeftIndex.X * TileSize, TopLeftIndex.Y * TileSize));
	}
}

void UDSInventoryGridBase::RemoveItemWidgetFromSlot(FIntPoint TopLeftIndex)
{
	for (UWidget* ChildWidget : GridCanvasPanel->GetAllChildren())
	{
		// 꾩씠쒖쓽 꾩튂 쇱튂섎뒗 寃쎌슦 쒓굅
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ChildWidget->Slot);
		if (IsValid(CanvasSlot))
		{
			FVector2D ItemPosition = CanvasSlot->GetPosition();
			if (ItemPosition.X == TopLeftIndex.X * TileSize && ItemPosition.Y == TopLeftIndex.Y * TileSize)
			{
				UDSInventoryItem* InventoryItem = Cast<UDSInventoryItem>(ChildWidget);
				if (IsValid(InventoryItem))
				{
					ChildWidget->RemoveFromParent();
					return;
				}

			}
		}
	}
}

void UDSInventoryGridBase::RegisterItemToAllItems()
{
	TSet<int32> OccupiedIndices;
	AllItems.Empty();

	const int32 Columns = InventoryInfo->Columns;
	const int32 Rows = InventoryInfo->Rows;

	for (int32 Index = 0; Index < Columns * Rows; ++Index)
	{
		if (OccupiedIndices.Contains(Index))
		{
			continue;
		}

		if (false == PersonalInventory->IsValidIndex(Index))
		{
			continue;
		}

		const FDSItemInfo& ItemSlot = (*PersonalInventory)[Index];

		if (ItemSlot.ID == 0)
		{
			continue;
		}

		UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

		check(DataManager);

		FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemSlot.ID));

		if (nullptr == ItemData)
		{
			continue;
		}

		FIntPoint Dimensions = ItemData->Dimensions;
		const FIntPoint FinalSize = ItemSlot.isRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;
		FIntPoint TopLeftTile = InventoryComponent->IndexToTile(Index, Columns);
		AllItems.Add(TopLeftTile, ItemSlot);

		for (int32 y = 0; y < FinalSize.Y; ++y)
		{
			for (int32 x = 0; x < FinalSize.X; ++x)
			{
				FIntPoint Tile;
				Tile.X = TopLeftTile.X + x;
				Tile.Y = TopLeftTile.Y + y;

				int32 OccupiedIndex = InventoryComponent->TiletoIndex(Tile, Columns);

				if (PersonalInventory->IsValidIndex(OccupiedIndex))
				{
					OccupiedIndices.Add(OccupiedIndex);
				}
			}
		}
	}
}

FIntPoint UDSInventoryGridBase::MousePositionInIntile(FVector2D MousePosition, bool bIsRotated)
{
	// 00 : 쇱そ 	// 10 : ㅻ뒛履	// 01 : 쇱そ 꾨옒
	// 11 : ㅻ뒛履꾨옒
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
