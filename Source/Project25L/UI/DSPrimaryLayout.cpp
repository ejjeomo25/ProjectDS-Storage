// Default
#include "UI/DSPrimaryLayout.h"

// UE
#include "GameFramework/PlayerController.h"

// Game
#include "System/DSUIManagerSubsystem.h"
#include "DSLogChannels.h"
#include "UI/DSWidgetLayer.h"

void UDSPrimaryLayout::NativeConstruct()
{
    Super::NativeConstruct();
    if (!GameMenuLayer)
    {
        GameMenuLayer = Cast<UDSWidgetLayer>(GetWidgetFromName(TEXT("GameMenuLayer")));
    }
    if (!ModalLayer)
    {
        ModalLayer = Cast<UDSWidgetLayer>(GetWidgetFromName(TEXT("ModalLayer")));
    }
	RegisterLayers();
}

void UDSPrimaryLayout::RegisterLayers()
{
    APlayerController* OwningPlayer = GetOwningPlayer();

    if (!IsValid(OwningPlayer))
	{
        return;
    }

    UDSUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UDSUIManagerSubsystem>();
    check(UIManager);
    
    UIManager->RegisterLayer(OwningPlayer, FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu")), GameMenuLayer);

    UIManager->RegisterLayer(OwningPlayer, FGameplayTag::RequestGameplayTag(FName("UI.Layer.Modal")), ModalLayer);
}
