//Default
#include "Player/DSPlayerController.h"

//UE
#include "EngineUtils.h"

//Game
#include "DSLogChannels.h"
#include "HUD/DSHUD.h"
#include "Input/DSPlayerInputComponent.h"
#include "Player/DSCheatManager.h"

ADSPlayerController::ADSPlayerController()
{
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent>(TEXT("DSPlayerInputComponent"));
	
#if USING_CHEAT_MANAGER
	CheatClass = UDSCheatManager::StaticClass();
#endif
}

void ADSPlayerController::PlayDoorTransition()
{
	//클라이언트에서만 실행되어진다.
	ADSHUD* HUD = GetHUD<ADSHUD>();

	if (IsValid(HUD))
	{
		HUD->SetVisibilityWidget(EWidgetType::HUBMainWidget, true);
	}
}

void ADSPlayerController::ServerRPC_Cheat_Implementation(const FString& Message)
{
	if (CheatManager)
	{
		DS_NETLOG(DSNetLog, Warning, TEXT("ServerCheat: %s"), *Message);
		ClientMessage(ConsoleCommand(Message));
	}
}


void ADSPlayerController::ServerRPC_CheatAll_Implementation(const FString& Message)
{
	if (CheatManager)
	{
		UWorld* World = GetWorld();

		check(World);

		for (TActorIterator<ADSPlayerController> It(World); It; ++It)
		{
			ADSPlayerController* PC = (*It);

			if (PC)
			{
				DS_NETLOG(DSNetLog, Warning, TEXT("ServerCheatAll: %s"), *Message);
				PC->ClientMessage(PC->ConsoleCommand(Message));
			}
		}
	}
}

void ADSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	DSPlayerInputComponent->SetupInputComponent(InputComponent);
}

void ADSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	DSPlayerInputComponent->InitialCharacterSetting();
}

