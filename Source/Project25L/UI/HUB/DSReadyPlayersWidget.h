#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Blueprint/UserWidget.h"

// UHT
#include "DSReadyPlayersWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class PROJECT25L_API UDSReadyPlayersWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UDSReadyPlayersWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void IncreasePlayer();
	void DecreasePlayer();

protected:
	void UpdatePlayerCount();

protected:

	UPROPERTY(Transient)
	int32 ReadyPlayerCount;

	UPROPERTY(Transient)
	FString ReadyStatusText;

	UPROPERTY(EditDefaultsOnly, Category = Count)
	int32 MaxPlayer;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Count;

};
