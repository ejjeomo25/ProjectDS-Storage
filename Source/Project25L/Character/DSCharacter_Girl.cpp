// Default 
#include "Character/DSCharacter_Girl.h"

// UE
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"

// Game
#include "Skill/DSTestSkill.h"
#include "DSLogChannels.h"
#include "Skill/DSSkillControlComponent_Girl.h"


ADSCharacter_Girl::ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Girl>(TEXT("SkillControlComponent"));
}

void ADSCharacter_Girl::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ADSCharacter_Girl::AttackHitCheck()
{
}


