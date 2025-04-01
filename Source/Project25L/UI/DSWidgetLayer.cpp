// Default
#include "UI/DSWidgetLayer.h"

// UE
#include "Components/PanelWidget.h"

// Game
#include "DSLogChannels.h"

UDSWidgetLayer::UDSWidgetLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Stack()
{
}

UUserWidget* UDSWidgetLayer::PushWidget(FGameplayTag WidgetTag)
{
	// DS_LOG(DSUILog, Log, TEXT("UDSWidgetLayer::PushWidget %s"), *WidgetTag.ToString());
	UUserWidget* NewWidget = nullptr;

	if (CreatedWidgetsMap.Contains(WidgetTag))
	{
		NewWidget = CreatedWidgetsMap[WidgetTag];
	}
	else
	{
		TSoftClassPtr<UUserWidget> WidgetClass = FindWidget(WidgetTag);

		UClass* LoadedClass = WidgetClass.Get();
		if (false == IsValid(LoadedClass))
		{
			LoadedClass = WidgetClass.LoadSynchronous();
		}

		NewWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), LoadedClass);
		if (false == IsValid(NewWidget))
		{
			return nullptr;
		}

		CreatedWidgetsMap.Add(WidgetTag, NewWidget);
	}

	CollapseTopWidget();

	if (IsValid(Border))
	{
		Border->ClearChildren();
		Border->AddChild(NewWidget);
	}

	Stack.Add(NewWidget);
	
	ShowTopWidget();
	return NewWidget;

}

TSoftClassPtr<UUserWidget> UDSWidgetLayer::FindWidget(FGameplayTag WidgetTag)
{

	for (const auto& Pair : WidgetsMap)
	{
		if (WidgetTag.MatchesTagExact(Pair.Key))
		{
			return Pair.Value;
		}
	}
	return nullptr;
}

void UDSWidgetLayer::PopWidget()
{

	if (Stack.Num() == 0)
	{	
		return;
	}

	UUserWidget * TopWidget = GetTopWidget();

	if (IsValid(TopWidget))
	{
		TopWidget->SetVisibility(ESlateVisibility::Collapsed);

		if (Stack.Num() > 0)
		{
			Stack.RemoveAt(Stack.Num() - 1);
		}
	}
	Border->ClearChildren();

	UUserWidget* NewTopWidget = GetTopWidget();
	if (IsValid(NewTopWidget))
	{
		NewTopWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Border->AddChild(NewTopWidget);
	}
}

void UDSWidgetLayer::ClearStack()
{
	for (UUserWidget* Widget : Stack)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}

	Stack.Empty();

	if (IsValid(Border))
	{
		Border->ClearChildren();
	}
}

void UDSWidgetLayer::CollapseTopWidget()
{
	if (Stack.Num() > 0)
	{
		UUserWidget* TopWidget = Stack.Last();
		if (IsValid(TopWidget))
		{
			TopWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UDSWidgetLayer::ShowTopWidget()
{
	if (Stack.Num() > 0)
	{
		UUserWidget* TopWidget = Stack.Last();  
		if (IsValid(TopWidget))
		{
			TopWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

UUserWidget* UDSWidgetLayer::GetTopWidget()
{
	if (Stack.Num() == 0)
	{
		return nullptr;
	}

	return Stack.Last();
}
