#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

// UHT
#include "DSUIManagerSubsystem.generated.h"




 class UDSWidgetLayer;
 class DSPlayerController;

UCLASS()
class PROJECT25L_API UDSUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UDSUIManagerSubsystem();

	static UDSUIManagerSubsystem* Get(UObject* Object);

	bool RegisterLayer(const APlayerController* PlayerController, FGameplayTag LayerName, UDSWidgetLayer* LayerWidgett);
	UUserWidget* PushContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName, TSoftClassPtr<UUserWidget> WidgetClass);
	void PopContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName);
	void ClearLayer(const APlayerController* PlayerController, FGameplayTag LayerName);
	void FocusGame(APlayerController* PlayerController);
	void FocusModal(APlayerController* PlayerController);
	UUserWidget* PushContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName);

	UPROPERTY(EditAnywhere, Category = "Config")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> WidgetMap;
};
