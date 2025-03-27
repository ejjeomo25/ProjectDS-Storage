
// Default
#include "Skill/DSSkillControlComponent_Girl.h"

// UE
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

// Game
#include "DSLogChannels.h"

#include "Character/DSCharacterBase.h"
#include "GameData/Skill/DSComboActionData.h"

#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"

// Skill
#include "DSNoGearFlightSkill.h"

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

    DOREPLIFETIME(UDSSkillControlComponent_Girl, CanCombo);
    DOREPLIFETIME(UDSSkillControlComponent_Girl, CurrentCombo);
}

void UDSSkillControlComponent_Girl::PlayPunchComboAnimation(int32 ComboIndex)
{
    if(false == IsValid(SkillActorInfo->AnimInstance.Get())) 
    {
        return;
    }

    UAnimMontage* AnimMontage = PunchComboActionData->PunchMontage.LoadSynchronous();

    if (IsValid(AnimMontage))
    {
        SkillActorInfo->AnimInstance->Montage_Play(AnimMontage, ComboSpeedRate);
    }
}

void UDSSkillControlComponent_Girl::ActivateComboSkill()
{
    const int32 ComboIndex = 0;
    if (false == HasAuthority())
    {
        ServerRPC_PlayComboMontage(ComboIndex);
    }
    PlayPunchComboAnimation(ComboIndex);
}

void UDSSkillControlComponent_Girl::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitSkillActorInfo(InOwnerActor, InAvatarActor);

    SkillActorInfo->AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
    DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[ESkillType::MouseLSkill], this, &UDSSkillControlComponent_Girl::ActivateComboSkill);
    DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[ESkillType::FlightSkill], this, &UDSSkillControlComponent_Girl::ActivateFlightSkill);

    // FlightSkill 
    FDSSkillSpec FlightSkillSpec(UDSNoGearFlightSkill::StaticClass(), UDSGameUtils::GenerateUniqueSkillID());
    FDSSkillSpecHandle FlightSkillHandle = AddSkill(FlightSkillSpec);
}

void UDSSkillControlComponent_Girl::ActivateFlightSkill()
{
    FDSSkillSpec* SkilSpec = FindSkillSpecFromClass(UDSNoGearFlightSkill::StaticClass());

    TryActivateSkill(SkilSpec->Handle);
}

void UDSSkillControlComponent_Girl::ServerRPC_PlayComboMontage_Implementation(int32 ComboIndex)
{
    PlayPunchComboAnimation(ComboIndex);
    MulticastRPC_PlayComboMontage(ComboIndex);

}
void UDSSkillControlComponent_Girl::MulticastRPC_PlayComboMontage_Implementation(int32 ComboIndex)
{
    PlayPunchComboAnimation(0);
}
