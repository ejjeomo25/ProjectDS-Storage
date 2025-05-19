#include "Components/DSSpringArmComponent.h"

// Game
#include "DSLogChannels.h"

void UDSSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	DS_LOG(DSCameraLog, Log, TEXT("Update!!!"));
}
