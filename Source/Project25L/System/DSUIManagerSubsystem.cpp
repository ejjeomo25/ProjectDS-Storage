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
#include "UI/DSPrimaryLayout.h"

UDSUIManagerSubsystem::UDSUIManagerSubsystem():Super()
{
	
}

UDSUIManagerSubsystem* UDSUIManagerSubsystem::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UDSGameInstance* GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->GetSubsystem<UDSUIManagerSubsystem>();
}



UUserWidget* UDSUIManagerSubsystem::PushContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	
	UUserWidget* NewWidget = PrimaryWidget->PushContentToLayer(LayerName);

	if (NewWidget)
	{
		return NewWidget;
	}
	return nullptr;
}

void UDSUIManagerSubsystem::PopContentToLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	// if (IsValid(PlayerController))
	// {
	// 	AHUD* HUD = PlayerController->GetHUD();
	// 	if (IsValid(HUD))
	// 	{
	// 		ADSHUD* DSHUD = Cast<ADSHUD>(HUD);
	// 		if (LayersTop.Contains(LayerName))
	// 		{
	// 			UUserWidget* WidgetToPop = *LayersTop.Find(LayerName);
	// 
	// 			PrimaryWidget->PopContentfromLayer(LayerName);
	// 
	// 			LayersTop.Remove(LayerName);
	// 
	// 		}
	// 	}
	// }

	PrimaryWidget->PopContentfromLayer(LayerName);
}

void UDSUIManagerSubsystem::ClearLayer(const APlayerController* PlayerController, FGameplayTag LayerName)
{
	// AHUD* HUD = PlayerController->GetHUD();
	// if (IsValid(HUD))
	{
		// ADSHUD* DSHUD = Cast<ADSHUD>(HUD);
		PrimaryWidget->ClearLayer(LayerName);
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

bool UDSUIManagerSubsystem::RegisterWidget(UDSPrimaryLayout* Widget)
{
	if (false == IsValid(PrimaryWidget) && IsValid(Widget))
	{
		PrimaryWidget = Widget;
		return true;
	}
	return false;
}

