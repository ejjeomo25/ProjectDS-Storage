#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"


// UHT
#include "DSPrimaryLayout.generated.h"

class UDSWidgetLayer;

UCLASS()
class PROJECT25L_API UDSPrimaryLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UDSPrimaryLayout(const FObjectInitializer& ObjectInitializer);
public:
<<<<<<< HEAD
	virtual void NativeConstruct() override;
	
=======

	virtual void NativeConstruct() override;
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	UUserWidget* PushContentToLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	void PopContentfromLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	void ClearLayer(FGameplayTag WidgetTag);

	UFUNCTION(BlueprintCallable, Category = "DSSettings | Widgets")
	UDSWidgetLayer* FindLayerByTag(FGameplayTag WidgetTag);

	void RegisterLayers();

private:

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UDSWidgetLayer>> LayersMap;

	// 인벤토리 및 퀘스트 화면, 게임 플레이 메뉴
<<<<<<< HEAD
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSWidgetLayer> GameMenuLayer;
	
	// 팝업 및 대화상자
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptional))
	TObjectPtr<UDSWidgetLayer> ModalLayer;

	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UDSWidgetLayer> GameLayer;
=======
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UDSWidgetLayer> GameMenuLayer;
	
	// 팝업 및 대화상자
	UPROPERTY(EditAnywhere, meta = (BindWidgetOptions))
	TObjectPtr<UDSWidgetLayer> ModalLayer;

>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
};
