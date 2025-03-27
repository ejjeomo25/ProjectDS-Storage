#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "GameData/Items/DSItemData.h"

// UHT
#include "DSItemList.generated.h"

class UDSItemEntry;
class UDSListView;


UCLASS()
class PROJECT25L_API UDSItemList : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UDSItemList(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddItems(TArray<FDSItemInfo>& ItemInfos);
	void RemoveItem(int32 IndexToRemove);
protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	void FocusOnItemEntry(int PreSelectedIdx);
protected:
	UPROPERTY(Transient)
	int32 SelectedIdx;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSListView> ListView_Items;

};
