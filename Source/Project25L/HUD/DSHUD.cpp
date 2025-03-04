// Default
#include "HUD/DSHUD.h"

// UE
#include "Blueprint/UserWidget.h"

// Game

void ADSHUD::BeginPlay()
{
	Super::BeginPlay();

	InitializeWidgets();
}

void ADSHUD::SetVisibilityWidget(EWidgetType WidgetType, bool bShouldShow)
{
	if (Widgets.Contains(WidgetType))
	{
		if (bShouldShow)
		{
			Widgets[WidgetType]->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Widgets[WidgetType]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ADSHUD::InitializeWidgets()
{
	UWorld* World = GetWorld();
	check(World);

	for (const auto& WidgetClass : WidgetClasses)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(World, WidgetClass.Value);

		if (IsValid(Widget))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			Widget->AddToViewport(0);
			Widgets.Add(WidgetClass.Key, Widget);
		}
	}
}
