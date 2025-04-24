#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryItem.generated.h"


class UImage;
class USizeBox;
class UCanvasPanelSlot;
class UBorder;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemDragged, const FDSItemInfo&, FIntPoint /*TopLeftIndex*/);


UCLASS()
class PROJECT25L_API UDSInventoryItem : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnItemDragged OnItemDragged;

	// 마우스에 따라서 색 변경
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// 드래그 감지를 하기 위해 MouseButtonDown 함수 설정해줘야한다.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	void InitializeItem(const FDSItemInfo& InItemInfo, int32 InTileSize, FIntPoint TopLeftIndex);
	void Refresh();
	void GetItemMaterial(bool isRotated);

protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	int32 TileSize;

	UPROPERTY(Transient, BlueprintReadWrite)
	FIntPoint TopLeftIndex;

	UPROPERTY(Transient, BlueprintReadWrite)
	FVector2D Size;

	UPROPERTY(BlueprintReadOnly)
	FDSItemInfo ItemInfo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<USizeBox> BackgroundSizeBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> BackgroundBorder;
	
	UPROPERTY(EditDefaultsOnly, Category = "DSSettings")
	TSubclassOf<UDSInventoryItem> DragVisualClass;

};
