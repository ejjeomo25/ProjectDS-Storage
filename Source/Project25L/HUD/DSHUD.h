#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "GameFramework/HUD.h"

// Game
#include "System/DSEnums.h"


// UHT
#include "DSHUD.generated.h"

class UUserWidget;
class UDSWidgetLayer;
class UDSPrimaryLayout;




UCLASS()
class PROJECT25L_API ADSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	void SetVisibilityWidget(EWidgetType WigdetType, bool bShouldShow);

	void InitializeWidgets();

	void PushInitialScreens();

protected:

	// 레이어 저장되어 있는 맵
	// UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	// TMap<FGameplayTag, TSoftClassPtr<UDSWidgetLayer>> LayersMap;

	// 아이템 리스트 f 연속 눌렀을때 계속 둘어가서 확인용으로 넣었던 것
	// UPROPERTY()
	// TMap<FGameplayTag, UDSWidgetLayer*> UseLayersMap;

	// UPROPERTY(EditAnywhere, Category = "Config")
	// TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> InitialScreens;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDSPrimaryLayout> PrimaryLayoutClass;

};
