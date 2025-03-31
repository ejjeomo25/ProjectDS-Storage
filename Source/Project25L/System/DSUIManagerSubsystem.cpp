// Default
#include "System/DSUIManagerSubsystem.h"

// UE
#include "GameplayTagContainer.h"

// Game
#include "DSLogChannels.h"
#include "Player/DSPlayerController.h"
#include "System/DSGameInstance.h"
#include "UI/DSPrimaryLayout.h"


UDSUIManagerSubsystem::UDSUIManagerSubsystem()
	: Super()
	, PrimaryWidget(nullptr)
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



UUserWidget* UDSUIManagerSubsystem::PushContentToLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		UUserWidget* NewWidget = PrimaryWidget->PushContentToLayer(LayerName);

		if (IsValid(NewWidget))
		{
			return NewWidget;
		}
	}	
	
	return nullptr;
}

void UDSUIManagerSubsystem::PopContentToLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		PrimaryWidget->PopContentfromLayer(LayerName);
	}
}

void UDSUIManagerSubsystem::ClearLayer(FGameplayTag LayerName)
{
	if (IsValid(PrimaryWidget))
	{
		PrimaryWidget->ClearLayer(LayerName);
	}
}

void UDSUIManagerSubsystem::FocusGame(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);

		if (IsValid(DSPlayerController))
		{
			DSPlayerController->SetGameFocusMode();
		}
	}	
}

void UDSUIManagerSubsystem::FocusModal(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(PlayerController);

		if (IsValid(DSPlayerController))
		{
			DSPlayerController->SetUIFocusMode();
		}
	}
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

