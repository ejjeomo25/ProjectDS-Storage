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
	UDSWidgetLayer(const FObjectInitializer& ObjectInitializer);

	UUserWidget* PushWidget(FGameplayTag WidgetTag);
	TSoftClassPtr<UUserWidget> FindWidget(FGameplayTag WidgetTag);
	void PopWidget();
	void ClearStack();
	void CollapseTopWidget();
	void ShowTopWidget();
	UUserWidget* GetTopWidget();

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Border;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UUserWidget>> Stack;


	// 위젯이 각각 가지고 있음 Tag로 찾음
	UPROPERTY(EditAnywhere, Category = "DSSettings | Widget")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> WidgetsMap;

	// 생성된 위젯 
	UPROPERTY(EditAnywhere, Category = "DSSettings | Widget")
	TMap<FGameplayTag, UUserWidget*> CreatedWidgetsMap;
};
