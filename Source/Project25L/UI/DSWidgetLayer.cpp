// Default
#include "UI/DSWidgetLayer.h"

// UE
#include "Components/PanelWidget.h"

// Game
#include "DSLogChannels.h"

UUserWidget* UDSWidgetLayer::PushWidget(TSoftClassPtr<UUserWidget> WidgetClass)
{
	if (!WidgetClass)
	{
		return nullptr;
	}


	// 동기적으로 로드
	UClass* LoadedWidgetClass = WidgetClass.LoadSynchronous();
	if (!LoadedWidgetClass) 
	{
		return nullptr;
	}

	// UUserWidget 클래스로 캐스팅
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), LoadedWidgetClass);
	if (!NewWidget) 
	{
		return nullptr;
	}

	CollapseTopWidget();

	if (Border)
	{
		Border->ClearChildren();
	}

	Stack.Add(NewWidget);

	Border->AddChild(NewWidget);

	ShowTopWidget();
	return NewWidget;


}

void UDSWidgetLayer::PopWidget()
{
	if(Stack.Num() == 0)
		return ;
	UUserWidget * TopWidget = GetTopWidget();

	if (IsValid(TopWidget))
	{
		TopWidget->RemoveFromParent();
		Stack.RemoveAt(Stack.Num() - 1);
		Border->ClearChildren();
	}

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

void UDSWidgetLayer::NativeConstruct()
{
	DS_LOG(DSUILog, Log, TEXT("UDSWidgetLayer::NativeConstruct()"));
}


