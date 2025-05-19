#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

<<<<<<< HEAD
// Game
#include "GameData/Items/DSItemData.h"

=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
// UHT
#include "DSInventoryGridWidget.generated.h"


<<<<<<< HEAD
class UDSInventoryComponent;
class UDSInventoryItem;
class UPanelWidget;
class UCanvasPanel;
class UBorder;

=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
UCLASS()
class PROJECT25L_API UDSInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()
	
<<<<<<< HEAD
public:
	UDSInventoryGridWidget(const FObjectInitializer& ObjectInitializer);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

	UFUNCTION()
	void Init(UDSInventoryComponent* InventoryComponent, int InTileSize);

	// 선 만들어 주는 함수
	void CreateLineSegments();

	void Refresh();


protected:
	UFUNCTION()
	FIntPoint  MousePositionInIntile(FVector2D MousePosition, bool bIsRotated);

	UPROPERTY(Transient)
	TObjectPtr<UDSInventoryComponent> Inventory;

	UPROPERTY(Transient, BlueprintReadWrite)
	int TileSize;

	UPROPERTY(meta = (BindWidgetOptional),BlueprintReadWrite)
	TObjectPtr<UPanelWidget> GridBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadWrite)
	TObjectPtr<UCanvasPanel> GridCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDSLine> Lines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DrawDropLocation : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDSInventoryItem> InventoryItemClass;


=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
};
