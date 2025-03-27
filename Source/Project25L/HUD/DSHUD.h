#pragma once

// Defualt
#include "CoreMinimal.h"

// UE
#include "GameFramework/HUD.h"

// Game
#include "System/DSEnums.h"
#include "GameplayTagContainer.h"


// UHT
#include "DSHUD.generated.h"

class UUserWidget;
class UDSWidgetLayer;
class UDSPrimaryLayout;


 USTRUCT(BlueprintType)
 struct FDSWidgetStruct
 {
	 GENERATED_BODY()

	 UPROPERTY(EditAnywhere)
	 TSubclassOf<UUserWidget> Widget;

	 UPROPERTY(EditAnywhere)
	 FGameplayTag Tag;
 };


UCLASS()
class PROJECT25L_API ADSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	void SetVisibilityWidget(EWidgetType WigdetType, bool bShouldShow);

	void InitializeWidgets();

	void PushInitialScreens();

	// UI Manager
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	UUserWidget* PushContentToLayer(FGameplayTag LayerName, TSoftClassPtr<UUserWidget> SoftWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void PopContentfromLayer(FGameplayTag LayerName);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	bool RegisterLayer(FGameplayTag LayerName, UDSWidgetLayer* LayerWidget);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ClearLayer(FGameplayTag LayerName);

protected:

	UPROPERTY(Transient)
	TMap<EWidgetType, TObjectPtr<UUserWidget>> Widgets;

	UPROPERTY(EditAnywhere, Category = Widgets)
	TMap<EWidgetType, TSubclassOf<UUserWidget>> WidgetClasses;

	//UI Stack
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TArray<FGameplayTag> WidgetTagArray;

	UPROPERTY()
	TMap<FGameplayTag, UDSWidgetLayer*> LayersMap;

	UPROPERTY()
	TMap<FGameplayTag, UDSWidgetLayer*> UseLayersMap;

	UPROPERTY(EditAnywhere, Category = "Config")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> InitialScreens;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSoftClassPtr<UDSPrimaryLayout> PrimaryLayoutClass;

};
