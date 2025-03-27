#pragma once

// Default
#include "CoreMinimal.h"

//Game
#include "Skill/DSSkillBase.h"
#include "System/DSEnums.h"

//UHT
#include "DSFlightSkillBase.generated.h"


/**
 * 
 */
 // Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FFlightStateChanged, EFlightState);

UCLASS()
class PROJECT25L_API UDSFlightSkillBase : public UDSSkillBase
{
	GENERATED_BODY()
	
public:
	UDSFlightSkillBase() { }
	UDSFlightSkillBase(const FObjectInitializer& ObjectInitializer);

	FFlightStateChanged OnFlightStateChangedEvnet;

	virtual void BeginFlight() { }
	virtual void TickFlight(float DeltaTimes) { }
	virtual void EndFlight() { }

protected:
	virtual void OnSkillInitialized() override;
	virtual void OnSkillActivated() override;
	virtual void OnSkillDeactivated() override;

	virtual void SetSkillState(EFlightState NewState);

private:
	EFlightState CurrentFlightState;
};
