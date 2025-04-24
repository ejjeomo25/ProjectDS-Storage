#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSItemTooltip.generated.h"

class UTextBlock;

UCLASS()
class PROJECT25L_API UDSItemTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDSItemTooltip(const FObjectInitializer& ObjectInitializer);

	void SetText(int32 ItemID);
protected:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_ItemName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Tooltip;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Stat;

};
