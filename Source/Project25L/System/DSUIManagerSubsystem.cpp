// Default
#include "System/DSUIManagerSubsystem.h"

// UE
#include "GameplayTagContainer.h"

// Game
#include "HUD/DSHUD.h"
#include "DSLogChannels.h"
#include "UI/DSWidgetLayer.h"
#include "System/DSGameInstance.h"
#include "Player/DSPlayerController.h"

UDSUIManagerSubsystem::UDSUIManagerSubsystem():Super()
{
	// test -> 게임데이터 에셋으로 넣기 
	FString WidgetPath = "/Game/UI/Item/WBP_ItemList.WBP_ItemList_C";
	FSoftClassPath SoftClassPath(WidgetPath);
	TSoftClassPtr<UUserWidget> WidgetPtr(SoftClassPath);
	FGameplayTag LayerTag = FGameplayTag::RequestGameplayTag("UI.Layer.Modal");
	WidgetMap.Add(LayerTag, WidgetPtr);
}

UDSUIManagerSubsystem* UDSUIManagerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UDSGameInstance* GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->GetSubsystem<UDSUIManagerSubsystem>();
}

bool UDSUIManagerSubsystem::RegisterLayer(const APlayerController* PlayerController, FGameplayTag LayerName, UDSWidgetLayer* LayerWidget)
{
	if (!PlayerController) return false;

	ADSHUD* HUD = Cast<ADSHUD>(PlayerController->GetHUD());
	if (HUD)
	{
		return HUD->RegisterLayer(LayerName, LayerWidget);
	}

	return false;
}

UUserWidget* UDSUIManagerSubsystem::PushContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName, TSoftClassPtr<UUserWidget> WidgetClass)
{
	
	DS_LOG(DSUILog, Log, TEXT("UDSUIManagerSubsystem::PushContentToLayer"));
	if (IsValid(PlayerController))
	{
		DS_LOG(DSUILog, Log, TEXT("UDSUIManagerSubsystem::PushContentToLayer :: PlayerController"));

		AHUD* HUD = PlayerController->GetHUD();
		if (IsValid(HUD))
		{
			DS_LOG(DSUILog, Log, TEXT("UDSUIManagerSubsystem::PushContentToLayer :: HUD"));

			ADSHUD* DSHUD = Cast<ADSHUD>(HUD);
			return DSHUD->PushContentToLayer(LayerName, WidgetClass);
		}
	}
	return nullptr;
}

void UDSUIManagerSubsystem::PopContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	if (IsValid(PlayerController))
	{
		AHUD* HUD = PlayerController->GetHUD();
		if (IsValid(HUD))
		{
			ADSHUD* DSHUD = Cast<ADSHUD>(HUD);
			DSHUD->PopContentfromLayer(LayerName);
		}
	}
}

void UDSUIManagerSubsystem::ClearLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	AHUD* HUD = PlayerController->GetHUD();
	if (IsValid(HUD))
	{
		ADSHUD* DSHUD = Cast<ADSHUD>(HUD);
		DSHUD->ClearLayer(LayerName);
	}
}

void UDSUIManagerSubsystem::FocusGame(APlayerController* PlayerController)
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);
	DSPlayerController->SetGameFocusMode();
}

void UDSUIManagerSubsystem::FocusModal(APlayerController* PlayerController)
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);
	DSPlayerController->SetUIFocusMode();
}

UUserWidget* UDSUIManagerSubsystem::PushContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	// if (!PlayerController)
	// {
	//     UE_LOG(LogTemp, Warning, TEXT("PushContentToLayer: PlayerController is nullptr!"));
	//     return nullptr;
	// }
	// 
	// FGameplayTag LayerTag = FGameplayTag::RequestGameplayTag(LayerName);
	// 
	// if (WidgetMap.Contains(LayerTag))
	// {
	//     // TSoftClassPtr<UUserWidget> 가져오기
	//     TSoftClassPtr<UUserWidget> WidgetPtr = WidgetMap[LayerTag];
	// 
	//     // 클래스가 로드되어 있는지 확인
	//     if (false == WidgetPtr.IsValid())
	//     {
	// 		UE_LOG(LogTemp, Warning, TEXT("PushContentToLayer: here!!!!"));
	// 		return PushContentToLayer(PlayerController, LayerName, WidgetPtr.LoadSynchronous());
	//     }
	// 	else
	// 	{
	// 		return PushContentToLayer(PlayerController, LayerName, WidgetPtr.Get());
	// 	}
	// }
	// 
	return nullptr;
}