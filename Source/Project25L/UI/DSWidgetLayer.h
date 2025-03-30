#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"

// UHT
#include "DSWidgetLayer.generated.h"

class UPanelWidget;

UCLASS()
class PROJECT25L_API UDSWidgetLayer : public UUserWidget
{
	GENERATED_BODY()

public:
	UUserWidget* PushWidget(FGameplayTag WidgetTag);
	TSoftClassPtr<UUserWidget> FindWidget(FGameplayTag WidgetTag);
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


	// 위젯이 각각 가지고 있음 Tag로 찾음
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> WidgetsMap;

	// 생성된 위젯 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<FGameplayTag, UUserWidget*> CreatedWidgetsMap;
};
