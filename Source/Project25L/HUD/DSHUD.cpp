// Default
#include "HUD/DSHUD.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "DSLogChannels.h"
#include "UI/DSPrimaryLayout.h"
#include "System/DSUIManagerSubsystem.h"

#include "UI/DSWidgetLayer.h"

void ADSHUD::BeginPlay()
{
	Super::BeginPlay();

	InitializeWidgets();
}

void ADSHUD::SetVisibilityWidget(EWidgetType WidgetType, bool bShouldShow)
{
	// if (Widgets.Contains(WidgetType))
	// {
	// 	if (bShouldShow)
	// 	{
	// 		Widgets[WidgetType]->SetVisibility(ESlateVisibility::HitTestInvisible);
	// 	}
	// 	else
	// 	{
	// 		Widgets[WidgetType]->SetVisibility(ESlateVisibility::Collapsed);
	// 	}
	// }
}

void ADSHUD::InitializeWidgets()
{
	UWorld* World = GetWorld();
	check(World);

	APlayerController* OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer && PrimaryLayoutClass.Get() && OwningPlayer->IsLocalController())
	{
		// 블루프린트 위젯 생성
		UDSPrimaryLayout* PrimaryLayout = CreateWidget<UDSPrimaryLayout>(OwningPlayer, PrimaryLayoutClass);
		if (PrimaryLayout)
		{
			PrimaryLayout->AddToViewport(); // 뷰포트에 추가
			UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
			check(UIManager);
			UIManager->RegisterWidget(PrimaryLayout);
		}
	}
}

