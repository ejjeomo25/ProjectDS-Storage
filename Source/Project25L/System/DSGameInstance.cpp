// Default
#include "System/DSGameInstance.h"

// UE

// Game
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"

void UDSGameInstance::Init()
{
	Super::Init();

	EventSystem = NewObject<UDSEventSystems>();

	GameUtils = NewObject<UDSGameUtils>();
}
