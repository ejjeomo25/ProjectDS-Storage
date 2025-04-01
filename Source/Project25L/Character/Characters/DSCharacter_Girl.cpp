// Default 
#include "Character/Characters/DSCharacter_Girl.h"

// UE
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"

// Game
#include "Skill/Base/DSTestSkill.h"
#include "DSLogChannels.h"
#include "Components/Skill/DSSkillControlComponent_Girl.h"


ADSCharacter_Girl::ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SkillControlComponent = CreateDefaultSubobject<UDSSkillControlComponent_Girl>(TEXT("SkillControlComponent"));
}

void ADSCharacter_Girl::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}




