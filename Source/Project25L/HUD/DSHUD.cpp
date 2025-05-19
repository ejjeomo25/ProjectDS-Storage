// Default
#include "HUD/DSHUD.h"

// UE
#include "Blueprint/UserWidget.h"

// Game
#include "UI/Base/DSPrimaryLayout.h"
#include "System/DSUIManagerSubsystem.h"


void ADSHUD::BeginPlay()
{
	Super::BeginPlay();
	InitializeWidgets();

}


void ADSHUD::InitializeWidgets()
{
	APlayerController* OwningPlayer = GetOwningPlayerController();
	if (OwningPlayer && PrimaryLayoutClass.Get() && OwningPlayer->IsLocalController())
	{ 
		UDSPrimaryLayout* PrimaryLayout = CreateWidget<UDSPrimaryLayout>(OwningPlayer, PrimaryLayoutClass);
		if (PrimaryLayout)
		{
			PrimaryLayout->AddToViewport(); 
			UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
			check(UIManager);
			UIManager->RegisterWidget(PrimaryLayout);
		}
	}
}



