#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"


// UHT
#include "DSPrimaryLayout.generated.h"

/**
 * 
 */
class UUserWidget;
class UDSWidgetLayer;

UCLASS()
class PROJECT25L_API UDSPrimaryLayout : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	UUserWidget* PushContentToLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void PopContentfromLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ClearLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	UDSWidgetLayer* FindLayerByTag(FGameplayTag WidgetTag);

	void RegisterLayers();

private:

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UDSWidgetLayer>> LayersMap;


	// 인벤토리 및 퀘스트 화면, 게임 플레이 메뉴
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UDSWidgetLayer> GameMenuLayer;
	
	// 팝업 및 대화상자
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UDSWidgetLayer> ModalLayer;

};
