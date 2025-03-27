#pragma once

// Default
#include "CoreMinimal.h"

//Game
#include "Skill/DSFlightSkillBase.h"

//UHT
#include "DSNoGearFlightSkill.generated.h"

/**
 * 
 */

UCLASS()
class PROJECT25L_API UDSNoGearFlightSkill : public UDSFlightSkillBase
{
	GENERATED_BODY()

public:
	UDSNoGearFlightSkill(const FObjectInitializer& ObjectInitializer);

	virtual void BeginFlight();
	virtual void TickFlight(float DeltaTimes);
	virtual void EndFlight() ;
};
