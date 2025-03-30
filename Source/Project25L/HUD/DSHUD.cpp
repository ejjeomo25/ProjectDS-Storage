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
			// PushInitialScreens();
		}
	}
}

void ADSHUD::PushInitialScreens()
{
	// APlayerController* PlayerController = GetOwningPlayerController();
	// if (!PlayerController)
	// {
	// 	DS_LOG(DSUILog, Log, TEXT("PlayerController is nullptr!"));
	// 	return;
	// }
	// 
	// // UI Manager Subsystem 가져오기
	// UGameInstance* GameInstance = GetGameInstance();
	// if (!GameInstance)
	// {
	// 	DS_LOG(DSUILog, Log, TEXT("GameInstance is nullptr!"));
	// 	return;
	// }
	// 
	// UDSUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UDSUIManagerSubsystem>();
	// if (!UIManager)
	// {
	// 	DS_LOG(DSUILog, Log, TEXT("UIManager is nullptr!"));
	// 	return;
	// }
	// 
	// // TMap을 직접 순회 (FGameplayTag 기반)
	// for (const TPair<FGameplayTag, TSoftClassPtr<UUserWidget>>& Pair : InitialScreens)
	// {
	// 	FGameplayTag LayerTag = Pair.Key;
	// 	TSoftClassPtr<UUserWidget> SoftWidgetClass = Pair.Value;
	// 
	// 	// 소프트 클래스 로딩
	// 	if (!SoftWidgetClass.IsNull())
	// 	{
	// 		TSoftClassPtr<UUserWidget> WidgetClass = SoftWidgetClass.LoadSynchronous();
	// 		if (WidgetClass)
	// 		{
	// 			UIManager->PushContentToLayer(PlayerController, LayerTag);
	// 			DS_LOG(DSUILog, Log, TEXT("Added widget %s to layer %s"), *WidgetClass->GetName(), *LayerTag.ToString());
	// 		}
	// 		else
	// 		{
	// 			DS_LOG(DSUILog, Log, TEXT("Failed to load widget for layer: %s"), *LayerTag.ToString());
	// 		}
	// 	}
	// }
}

