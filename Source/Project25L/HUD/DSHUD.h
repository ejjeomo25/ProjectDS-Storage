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

UCLASS()
class PROJECT25L_API ADSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	void SetVisibilityWidget(EWidgetType WigdetType, bool bShouldShow);

	void InitializeWidgets();

protected:

	UPROPERTY(Transient)
	TMap<EWidgetType, TObjectPtr<UUserWidget>> Widgets;

	UPROPERTY(EditAnywhere, Category = Widgets)
	TMap<EWidgetType, TSubclassOf<UUserWidget>> WidgetClasses;

};
