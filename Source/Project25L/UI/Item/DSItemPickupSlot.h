#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSItemPickupSlot.generated.h"

class UImage;
class UTextBlock;
class UDSInventoryComponent;

UCLASS()
class PROJECT25L_API UDSItemPickupSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UDSItemPickupSlot(const FObjectInitializer& ObjectInitializer);

	void InvalidateWidget(UDSInventoryComponent* Inventory);

protected:
	void UpdateWidget(int32 ItemID, bool bRemoved);

	void DisappearSlot();
protected:

	UPROPERTY(Transient)
	FTimerHandle ItemSlotExpireHandle;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Alert;
};
