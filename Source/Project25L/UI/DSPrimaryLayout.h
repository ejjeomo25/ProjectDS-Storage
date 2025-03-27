#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

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

private:
	void RegisterLayers();

	// 인벤토리 및 퀘스트 화면, 게임 플레이 메뉴
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UDSWidgetLayer> GameMenuLayer;
	
	// 팝업 및 대화상자
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UDSWidgetLayer> ModalLayer;
};
