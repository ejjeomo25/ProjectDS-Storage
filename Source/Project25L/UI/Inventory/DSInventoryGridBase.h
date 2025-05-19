#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "UI/Base/DSUserWidget.h"
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSInventoryGridBase.generated.h"

class UDSInventoryComponent;
class UDSInventoryItem;
class UPanelWidget;
class UCanvasPanel;
class UBorder;

UCLASS()
class PROJECT25L_API UDSInventoryGridBase : public UDSUserWidget
{
	GENERATED_BODY()

public:
	UDSInventoryGridBase(const FObjectInitializer& ObjectInitializer);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	void Init(EInventoryCategory InInventoryCategory, FPersonalInventoryInfo* InInventoryInfo, TArray<FDSItemInfo>* InPersonalInventory, UDSInventoryComponent* InInventoryComponent);

	// 선 만들어 주는 함수
	void CreateLineSegments();

	void Refresh(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex, bool bRemoved);
	virtual void AddItemWidgetToSlot(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex);
	void RemoveItemWidgetFromSlot(FIntPoint TopLeftIndex);
	void RegisterItemToAllItems();

	UFUNCTION()
	FIntPoint  MousePositionInIntile(FVector2D MousePosition, bool bIsRotated);


protected:
	UPROPERTY(Transient)
	EInventoryCategory InventoryCategory;

	UPROPERTY(Transient)
	TObjectPtr<UDSInventoryComponent> InventoryComponent;

	UPROPERTY(Transient, BlueprintReadWrite)
	int TileSize;

	FPersonalInventoryInfo* InventoryInfo;

	TArray<FDSItemInfo>* PersonalInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, FDSItemInfo> AllItems;

	// UI
	UPROPERTY(meta = (BindWidgetOptional),BlueprintReadWrite)
	TObjectPtr<UPanelWidget> GridBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<UCanvasPanel> GridCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDSLine> Lines;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDSInventoryItem> InventoryItemClass;
	
};
