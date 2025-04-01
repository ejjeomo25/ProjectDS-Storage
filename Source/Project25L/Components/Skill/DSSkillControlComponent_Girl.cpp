
// Default
#include "Components/Skill/DSSkillControlComponent_Girl.h"

// UE
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "GameData/Skill/DSComboActionData.h"
#include "Skill/Girl/DSPrimarySkill.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"

#include "DSLogChannels.h"

UDSSkillControlComponent_Girl::UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
}

void UDSSkillControlComponent_Girl::InitializeComponent()
{
    Super::InitializeComponent();

    SetIsReplicated(true);
}

void UDSSkillControlComponent_Girl::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UDSSkillControlComponent_Girl::ActivatePrimarySKill()
{
    ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

    if (IsValid(Character))
    {
        bool bIsEquipped = Character->GetIsEquipped();

        //장착이 안된 상태에서 스킬은 발사되지 않는다.
        if (bIsEquipped == false)
        {
            return;
        }

		FDSSkillSpec* SkillSpec = FindSkillSpecFromType(ESkillType::PrimarySkill);
        
		if (nullptr != SkillSpec)
		{
			TryActivateSkill(SkillSpec->Handle);
		}
    }
}

void UDSSkillControlComponent_Girl::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitSkillActorInfo(InOwnerActor, InAvatarActor);

	SkillActorInfo->AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();

    if(false == HasAuthority())
    {
        FDSSkillSpec PrimarySkillSpec(UDSPrimarySkill::StaticClass(), ESkillType::PrimarySkill, UDSGameUtils::GenerateUniqueSkillID());
        PrimarySkillSpec.bActivateOnce = true;
        PrimarySkillSpec.SkillType = ESkillType::PrimarySkill;
        AddSkill(PrimarySkillSpec);
    }
}
