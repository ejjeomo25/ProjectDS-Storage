#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSReadyPlayersWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECT25L_API UDSReadyPlayersWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UDSReadyPlayersWidget(const FObjectInitializer& ObjectInitializer);

	void UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount);

protected:


	UPROPERTY(EditAnywhere, Category = "DSSettings | Text")
	FString ReadyStatusText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Count;

};
