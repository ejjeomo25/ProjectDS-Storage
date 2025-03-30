
// Default
#include "Skill/DSSkillControlComponent_Girl.h"

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

    DOREPLIFETIME(UDSSkillControlComponent_Girl, CanCombo);
    DOREPLIFETIME(UDSSkillControlComponent_Girl, CurrentCombo);
}

void UDSSkillControlComponent_Girl::PlayPunchComboAnimation(int32 ComboIndex)
{
    if(false == IsValid(SkillActorInfo->AnimInstance.Get())) 
    {
        return;
    }

    if (false == IsValid(PunchComboActionData))
    {
        return;
    }

    UAnimMontage* AnimMontage = PunchComboActionData->PunchMontage.LoadSynchronous();

    if (IsValid(AnimMontage))
    {
        SkillActorInfo->AnimInstance->Montage_Play(AnimMontage, ComboSpeedRate);
    }
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

        FDSSkillSpec* SkilSpec = FindSkillSpecFromHandle(HasSkills[ESkillType::MouseLSkill]);
        if (nullptr != SkilSpec)
        {
            DS_NETLOG(DSNetLog, Log, TEXT("KKKKK Activate"));

            TryActivateSkill(SkilSpec->Handle);
        }
    }
}

void UDSSkillControlComponent_Girl::ActivateComboSkill()
{
    ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

    if (IsValid(Character))
    {
        bool bIsEquipped = Character->GetIsEquipped();

        //μ갑덈맂 곹깭먯꽌 ㅽ궗 諛쒖궗섏 딅뒗
        if (bIsEquipped == false)
        {
            return;
        }

        DS_NETLOG(DSNetLog, Log, TEXT("KKKKK Activate"));
        
        const int32 ComboIndex = 0;
        if (false == HasAuthority())
        {
            ServerRPC_PlayComboMontage(ComboIndex);
        }
        PlayPunchComboAnimation(ComboIndex);
    }
}

void UDSSkillControlComponent_Girl::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitSkillActorInfo(InOwnerActor, InAvatarActor);

    SkillActorInfo->AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
  
    DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[ESkillType::MouseLSkill], this, &UDSSkillControlComponent_Girl::ActivatePrimarySKill);
    
    FDSSkillSpec PrimarySkillSpec(UDSPrimarySkill::StaticClass(), UDSGameUtils::GenerateUniqueSkillID());
    PrimarySkillSpec.bActivateOnce = true;
    PrimarySkillSpec.SkillType = ESkillType::MouseLSkill;

    FDSSkillSpecHandle SpecHandle = AddSkill(PrimarySkillSpec);

    HasSkills.Add(ESkillType::MouseLSkill, SpecHandle);
}

void UDSSkillControlComponent_Girl::ActivateFlightSkill()
{
    // FDSSkillSpec* SkilSpec = FindSkillSpecFromClass(UDSFlightSkill::StaticClass());
    // TryActivateSkill(SkilSpec->Handle);
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
