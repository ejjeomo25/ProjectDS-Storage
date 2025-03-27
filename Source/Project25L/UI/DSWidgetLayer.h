#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSWidgetLayer.generated.h"

class UPanelWidget;

UCLASS()
class PROJECT25L_API UDSWidgetLayer : public UUserWidget
{
	GENERATED_BODY()

public:
	UUserWidget* PushWidget(TSoftClassPtr<UUserWidget> WidgetClass);
	void PopWidget();
	void ClearStack();
	void CollapseTopWidget();
	void ShowTopWidget();
	UUserWidget* GetTopWidget();

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Border;

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> Stack;
};
