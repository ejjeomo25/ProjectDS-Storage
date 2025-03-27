// Default
#include "Skill/DSFlightSkillBase.h"

// Game
#include "System/DSEventSystems.h"
#include "Player/DSPlayerController.h"
#include "Input/DSPlayerInputComponent.h"

UDSFlightSkillBase::UDSFlightSkillBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void UDSFlightSkillBase::OnSkillInitialized()
{
	Super::OnSkillInitialized();

	DSEVENT_DELEGATE_BIND(OnFlightStateChangedEvnet, this, &UDSFlightSkillBase::SetSkillState);
}

void UDSFlightSkillBase::OnSkillActivated()
{
	Super::OnSkillActivated();
}

void UDSFlightSkillBase::OnSkillDeactivated()
{
	Super::OnSkillDeactivated();

	ADSPlayerController* PlayerController = Cast< ADSPlayerController>(GetCurrentActorInfo()->PlayerController);
	if(false == IsValid(PlayerController))
	{
		return;
	}
	
	UDSPlayerInputComponent* PlayerInputComponent = PlayerController->GetPlayerInputComponent();
	if (false == IsValid(PlayerInputComponent))
	{
		return;
	}
	
	DSEVENT_DELEGATE_INVOKE(PlayerInputComponent->OnInputMappingChangedEvent, EInputMappingContextType::DefaultIMC);
}

void UDSFlightSkillBase::SetSkillState(EFlightState NewState)
{
	CurrentFlightState = NewState;
}
