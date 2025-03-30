// Default
#include "UI/HUB/DSReadyPlayersWidget.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

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

	if (IsValid(Text_Count))
	{
		Text_Count->SetText(FText::FromString(FormattedText));
	}
}
