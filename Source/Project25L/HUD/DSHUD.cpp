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

	APlayerController* OwningPlayer = GetOwningPlayerController();
	TSubclassOf<UDSPrimaryLayout> LayoutClass = PrimaryLayoutClass.LoadSynchronous();
	if (OwningPlayer && PrimaryLayoutClass.Get() && OwningPlayer->IsLocalController())
	{
		// 블루프린트 위젯 생성
		UDSPrimaryLayout* PrimaryLayout = CreateWidget<UDSPrimaryLayout>(OwningPlayer, LayoutClass);
		if (PrimaryLayout)
		{
			PrimaryLayout->AddToViewport(); // 뷰포트에 추가
			PushInitialScreens();
		}
	}
}

void ADSHUD::PushInitialScreens()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController)
	{
		DS_LOG(DSUILog, Log, TEXT("PlayerController is nullptr!"));
		return;
	}

	// UI Manager Subsystem 가져오기
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		DS_LOG(DSUILog, Log, TEXT("GameInstance is nullptr!"));
		return;
	}

	UDSUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UDSUIManagerSubsystem>();
	if (!UIManager)
	{
		DS_LOG(DSUILog, Log, TEXT("UIManager is nullptr!"));
		return;
	}

	// TMap을 직접 순회 (FGameplayTag 기반)
	for (const TPair<FGameplayTag, TSoftClassPtr<UUserWidget>>& Pair : InitialScreens)
	{
		FGameplayTag LayerTag = Pair.Key;
		TSoftClassPtr<UUserWidget> SoftWidgetClass = Pair.Value;

		// 소프트 클래스 로딩
		if (!SoftWidgetClass.IsNull())
		{
			TSoftClassPtr<UUserWidget> WidgetClass = SoftWidgetClass.LoadSynchronous();
			if (WidgetClass)
			{
				UIManager->PushContentToLayer(PlayerController, LayerTag, WidgetClass);
				DS_LOG(DSUILog, Log, TEXT("Added widget %s to layer %s"), *WidgetClass->GetName(), *LayerTag.ToString());
			}
			else
			{
				DS_LOG(DSUILog, Log, TEXT("Failed to load widget for layer: %s"), *LayerTag.ToString());
			}
		}
	}
}

UUserWidget* ADSHUD::PushContentToLayer(FGameplayTag LayerName, TSoftClassPtr<UUserWidget> SoftWidgetClass)
{
	DS_LOG(DSUILog, Log, TEXT("PushContentToLayer called with Layer: %s"), *LayerName.ToString());

	// 레이어 찾기
	UDSWidgetLayer** LayerPtr = LayersMap.Find(LayerName);
	if (!LayerPtr || !*LayerPtr) 
	{
		return nullptr;
	}

	if (UDSWidgetLayer** ExistingWidgetPtr = UseLayersMap.Find(LayerName))
	{
		if (*ExistingWidgetPtr)
		{
			return *ExistingWidgetPtr;
		}
	}

	// LoadSynchronous
	if (!SoftWidgetClass.IsNull())
	{
		TSoftClassPtr<UUserWidget> WidgetClass = SoftWidgetClass.LoadSynchronous();
		if (WidgetClass)
		{
			// PushWidget 실행
			UDSWidgetLayer* Layer = *LayerPtr;
			UUserWidget* NewWidget = Layer->PushWidget(WidgetClass);

			if (NewWidget)
			{
				UseLayersMap.Add(LayerName, Layer);
			}

			return NewWidget;
		}
	}

	return nullptr;
}

void ADSHUD::PopContentfromLayer(FGameplayTag LayerName)
{
	if (UDSWidgetLayer** FoundLayer = LayersMap.Find(LayerName))
	{
		if (FoundLayer == nullptr)  // 먼저 검사
		{
			DS_LOG(DSUILog, Log, TEXT("PopContentfromLayer: Layer %s not found in LayersMap!"), *LayerName.ToString());
			return;
		}
		UDSWidgetLayer* Layer = *FoundLayer;

		if (IsValid(Layer))
		{
			Layer->PopWidget();
			if (UseLayersMap.Contains(LayerName))
			{
				UseLayersMap.Remove(LayerName);
			}
			else
			{
				DS_LOG(DSUILog, Log, TEXT("PopContentfromLayer: Layer %s was not found in UseLayersMap"), *LayerName.ToString());
			}
		}
	}
}

bool ADSHUD::RegisterLayer(FGameplayTag LayerName, UDSWidgetLayer* LayerWidget)
{
	if (!LayerWidget || LayersMap.Contains(LayerName))
	{
		return false;
	}

	LayersMap.Add(LayerName, LayerWidget);
	return true;
}

void ADSHUD::ClearLayer(FGameplayTag LayerName)
{
	if (UDSWidgetLayer** FoundLayer = LayersMap.Find(LayerName))
	{
		UDSWidgetLayer* Layer = *FoundLayer;
		if (IsValid(Layer))
		{
			Layer->ClearStack();
		}
	}
}
