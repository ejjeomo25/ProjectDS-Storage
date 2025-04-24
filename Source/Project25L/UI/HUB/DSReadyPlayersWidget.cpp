// Default
#include "UI/HUB/DSReadyPlayersWidget.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "DSLogChannels.h"

UDSReadyPlayersWidget::UDSReadyPlayersWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ReadyStatusText("")
{
}


void UDSReadyPlayersWidget::UpdatePlayerCount(int CurrentPlayerCount, int MaxPlayerCount)
{
	FString FormattedText = FString::Format(*ReadyStatusText, { CurrentPlayerCount, MaxPlayerCount });

	if (IsValid(Text_Count))
	{
		Text_Count->SetText(FText::FromString(FormattedText));
	}
}
