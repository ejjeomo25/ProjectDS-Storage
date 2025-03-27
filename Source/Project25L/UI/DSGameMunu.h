#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"


// UHT
#include "DSGameMunu.generated.h"

/**
 * 
 */

class UWidgetSwitcher;
class UDSInventoryWidget;
class UBorder;

UCLASS()
class PROJECT25L_API UDSGameMunu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

    UFUNCTION()
    void ToggleWidget(UUserWidget* Widget);

protected:
    virtual void NativeConstruct() override;

    // 위젯 스위처 (인벤토리 On/Off)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

    // 특정 인벤토리 창 (예: Border)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UDSInventoryWidget> InventoryWidget;

        // 특정 인벤토리 창 (예: Border)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UBorder> EmptyBorder;
};
