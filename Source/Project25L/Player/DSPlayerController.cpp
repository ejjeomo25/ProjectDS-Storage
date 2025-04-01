//Default
#include "Player/DSPlayerController.h"

//UE
#include "EngineUtils.h"

//Game
#include "DSLogChannels.h"
#include "HUD/DSHUD.h"
#include "Player/DSCheatManager.h"

ADSPlayerController::ADSPlayerController()
{
#if USING_CHEAT_MANAGER
	CheatClass = UDSCheatManager::StaticClass();
#endif
}



void ADSPlayerController::SetUIFocusMode()
{
	bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	SetInputMode(InputMode);
}

void ADSPlayerController::SetGameFocusMode()
{
	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
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

