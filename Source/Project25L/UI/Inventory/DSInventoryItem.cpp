// Default
#include "UI/Inventory/DSInventoryItem.h"

// UE
#include "Components/SizeBox.h"
#include "components/Image.h"
#include "components/CanvasPanelSlot.h"
#include "Materials/MaterialInterface.h"
#include "components/Border.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "DSLogChannels.h"
#include "UI/Inventory/DSItemDragDropOperation.h"
#include "Components/DSInventoryComponent.h"



void UDSInventoryItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (IsValid(BackgroundBorder))
	{
		BackgroundBorder->SetBrushColor(FLinearColor(0.5, 0.5, 0.5, 0.2));
		// DS_LOG(DSUILog, Log, TEXT("NativeOnMouseEnter"));
	}
}

void UDSInventoryItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (IsValid(BackgroundBorder))
	{
		BackgroundBorder->SetBrushColor(FLinearColor(0.0, 0.0, 0.0, 0.5));
		// DS_LOG(DSUILog, Log, TEXT("NativeOnMouseLeave"));
	}
}

FReply UDSInventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply.Unhandled();
}



void UDSInventoryItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	if (IsValid(DragVisualClass))
	{
		UDSInventoryItem* DragVisual = CreateWidget<UDSInventoryItem>(this, DragVisualClass);
		DragVisual->InitializeItem(ItemInfo, TileSize, TopLeftIndex);
		DragVisual->SetRenderOpacity(0.5f);
		// 드래그 오퍼레이션 생성
		UDSItemDragDropOperation* DragOp = NewObject<UDSItemDragDropOperation>();

		// 아이템 정보 저장
		DragOp->ItemInfo = ItemInfo;
		DragOp->TopLeftIndex = TopLeftIndex;
		DragOp->DefaultDragVisual = DragVisual;
		DragOp->Pivot = EDragPivot::CenterCenter;
		OutOperation = DragOp;
	}
	
	DSEVENT_DELEGATE_INVOKE(OnItemDragged, ItemInfo, TopLeftIndex)
	RemoveFromParent();

}



void UDSInventoryItem::InitializeItem(const FDSItemInfo& InItemInfo, int32 InTileSize, FIntPoint InTopLeftIndex)
{
	ItemInfo = InItemInfo;
	TileSize = InTileSize;
	TopLeftIndex = InTopLeftIndex;
	GetItemMaterial(InItemInfo.isRotated);
	Refresh(); 
}

void UDSInventoryItem::Refresh()
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	// 사이즈 설정
	Size.X = FinalSize.X * TileSize;
	Size.Y = FinalSize.Y * TileSize;
	BackgroundSizeBox->SetWidthOverride(Size.X);
	BackgroundSizeBox->SetHeightOverride(Size.Y);
	if (IsValid(ItemImage))
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemImage->Slot);
		if (IsValid(CanvasSlot))
		{
			CanvasSlot->SetSize(Size);
		}
	}

}

void UDSInventoryItem::GetItemMaterial(bool bIsRotated)
{

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	UMaterialInterface* Material = nullptr;
	if (bIsRotated)
	{
		Material = ItemData->ItemTileMaterialRotated.LoadSynchronous();
	}
	else
	{
		Material = ItemData->ItemTileMaterial.LoadSynchronous();
	}

	if (false == IsValid(Material)) 
	{
		return;
	}

	FSlateBrush Brush;
	Brush.SetResourceObject(Material);
	Brush.ImageSize = FVector2D(Size.X, Size.Y);
	Brush.DrawAs = ESlateBrushDrawType::Image; 
	ItemImage->SetBrush(Brush);
}