
// Default
#include "Skill/DSNoGearFlightSkill.h"

UDSNoGearFlightSkill::UDSNoGearFlightSkill(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	SkillName = FName("NoGearFlightSkill");
	bSkillHasCooltime = false;

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

void UDSNoGearFlightSkill::BeginFlight()
{
}

void UDSNoGearFlightSkill::TickFlight(float DeltaTimes)
{
}

void UDSNoGearFlightSkill::EndFlight()
{
}
