#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSInventoryWidget.generated.h"

<<<<<<< HEAD

class UDSInventoryGridWidget;

=======
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
UCLASS()
class PROJECT25L_API UDSInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
<<<<<<< HEAD

public:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UDSInventoryGridWidget> InventoryGirdWidget;
=======
	
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
};
