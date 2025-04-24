
// Default
#include "Components/Skill/DSSkillControlComponent_Girl.h"

// UE
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "DSLogChannels.h"
#include "GameData/Skill/DSComboActionData.h"
#include "Skill/Girl/DSPrimarySkill_Girl.h"
#include "Skill/Girl/DSSkill1_Girl.h"
#include "Skill/Girl/DSSkill2_Girl.h"
#include "Skill/Girl/DSSkillQInstall_Girl.h"
#include "System/DSEventSystems.h"
#include "GameData/GameplayTag/DSGameplayTags.h"

UDSSkillControlComponent_Girl::UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
    , bHasInstalledSkill(false)
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
	FDSSkillSpec* SkillSpec = FindSkillSpecFromTag(FDSTags::GetDSTags().Skill_PrimarySkill);

	if (nullptr != SkillSpec)
	{
		TryActivateSkill(SkillSpec->Handle);
	}
}

void UDSSkillControlComponent_Girl::ActivateSKill1()
{
    FDSSkillSpec* SkillSpec = nullptr;
    if (bHasInstalledSkill)
    {
        SkillSpec = FindSkillSpecFromTag(FDSTags::GetDSTags().Skill_Skill1);
    }
    else
    {
        SkillSpec = FindSkillSpecFromTag(FDSTags::GetDSTags().Skill_Skill1_Installation);
    }

    if (nullptr != SkillSpec)
    {
        TryActivateSkill(SkillSpec->Handle);
    }
}

void UDSSkillControlComponent_Girl::ActivateSKill2()
{
    FDSSkillSpec* SkillSpec = FindSkillSpecFromTag(FDSTags::GetDSTags().Skill_Skill2);
	if (nullptr != SkillSpec)
	{
		TryActivateSkill(SkillSpec->Handle);
    }
}

void UDSSkillControlComponent_Girl::InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    Super::InitSkillActorInfo(InOwnerActor, InAvatarActor);

	SkillActorInfo->AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();

}

void UDSSkillControlComponent_Girl::InstallSkillTimer(float Sec, bool bIsCanceled)
{
    UWorld* World = GetWorld();
    check(World);

    if (bIsCanceled)
    {
        World->GetTimerManager().ClearTimer(InstallationSkillTimerHandle);
    }
    else
    {
        World->GetTimerManager().SetTimer(InstallationSkillTimerHandle, FTimerDelegate::CreateLambda([WeakObj = TWeakObjectPtr<UDSSkillControlComponent_Girl>(this)]
            {
                if (WeakObj.IsValid())
                {
                    WeakObj.Get()->OnExpiredSkill.ExecuteIfBound();
                }
            }), Sec, false);
    }
}
