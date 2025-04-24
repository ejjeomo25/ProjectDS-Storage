// Default
#include "UI/Game/DSMainWidget.h"

// UE
#include "Components/VerticalBox.h"

// Game
#include "UI/Game/Player/DS_HPBar.h"
#include "UI/Game/Player/DSPlayerInfo.h"

UDSMainWidget::UDSMainWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PlayerInfo))
	{
		//0번 Idx는 현재 플레이어 값
		PlayerInfos.Add(PlayerInfo);
	}

	if (IsValid(VBox_Info))
	{
		TArray<UWidget*> Widgets = VBox_Info->GetAllChildren();

		for (int WidgetIdx = 0; WidgetIdx < Widgets.Num(); WidgetIdx++)
		{
			UDSPlayerInfo* EntryInfo = Cast<UDSPlayerInfo>(Widgets[WidgetIdx]);

			if (IsValid(EntryInfo))
			{
				PlayerInfos.Add(EntryInfo);
			}
		}
	}
}

UDS_HPBar* UDSMainWidget::GetHPBar(int32 PlayerIndex)
{
	if (PlayerInfos.IsValidIndex(PlayerIndex))
	{
		//인덱스를 포함하고 있다면,

		UDSPlayerInfo* EntryInfo = PlayerInfos[PlayerIndex];

		if (IsValid(PlayerInfo))
		{
			return EntryInfo->GetHPBar();
		}
	}
	return nullptr;
}

UDSPlayerInfo* UDSMainWidget::GetPlayerInfo(int32 PlayerIndex)
{
	if (PlayerInfos.IsValidIndex(PlayerIndex))
	{
		//인덱스를 포함하고 있다면,

		UDSPlayerInfo* EntryInfo = PlayerInfos[PlayerIndex];

		if (IsValid(EntryInfo))
		{
			return EntryInfo;
		}
	}
	return nullptr;
}
