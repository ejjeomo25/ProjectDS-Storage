// Default
#include "System/DSGameInstance.h"

// UE

// Game
#include "System/DSEventSystems.h"

void UDSGameInstance::Init()
{
	Super::Init();

	EventSystem = NewObject<UDSEventSystems>();
}
