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

<<<<<<< HEAD
	void UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount);

protected:


	UPROPERTY(EditAnywhere, Category = "DSSettings | Text")
	FString ReadyStatusText;

=======
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

	UPROPERTY(EditDefaultsOnly, Category = "DSSettings | MaxPlayer")
	int32 MaxPlayer;

>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Count;

};
