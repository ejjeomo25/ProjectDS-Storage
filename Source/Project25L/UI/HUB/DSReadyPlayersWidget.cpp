// Default
#include "UI/HUB/DSReadyPlayersWidget.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

<<<<<<< HEAD
#include "DSLogChannels.h"

UDSReadyPlayersWidget::UDSReadyPlayersWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ReadyStatusText("")
{
}


void UDSReadyPlayersWidget::UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount)
{
	FString FormattedText = FString::Format(*ReadyStatusText, { CurrentPlayerCount, MaxPlayerCount });
=======
UDSReadyPlayersWidget::UDSReadyPlayersWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ReadyPlayerCount(0)
	, ReadyStatusText("")
	, MaxPlayer(3)
{
}

void UDSReadyPlayersWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Text_Count))
	{
		//Text를 가지고 와서 {0}/{1}에 MaxPlayer와 Player값으로 복사할 수 있도록 한다.
		ReadyStatusText = Text_Count->GetText().ToString(); 
	}
}

void UDSReadyPlayersWidget::IncreasePlayer()
{
	ReadyPlayerCount = FMath::Clamp(ReadyPlayerCount + 1, 0, MaxPlayer);
	UpdatePlayerCount();
}

void UDSReadyPlayersWidget::DecreasePlayer()
{
	ReadyPlayerCount = FMath::Clamp(ReadyPlayerCount-1, 0, MaxPlayer);
	UpdatePlayerCount();
}

void UDSReadyPlayersWidget::UpdatePlayerCount()
{
	FString FormattedText = FString::Format(*ReadyStatusText, { ReadyPlayerCount, MaxPlayer });
>>>>>>> 6d97a8e7eb31f849c4f7006f89fed6a9f4c3c463

	if (IsValid(Text_Count))
	{
		Text_Count->SetText(FText::FromString(FormattedText));
	}
}
