// Default 
#include "Character/DSCharacter_Girl.h"

// UE
#include "EngineUtils.h"

// Game
#include "Skill/DSTestSkill.h"
#include "Skill/DSSkillControlComponent.h"

#include "Character/DSCharacterMovementComponent.h"
#include "Player/DSPlayerController.h"
#include "Animation/DSBaseAnimInstance.h"

#include "Input/DSPlayerInputComponent.h"
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"

ADSCharacter_Girl::ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void ADSCharacter_Girl::InitSkillActorInfo()
{
	Super::InitSkillActorInfo();

	// 상위 클래스로 옮기기
	OnSkillPressedEvents.Add(ESkillType::MouseLSkill, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::MouseLHoldSkill, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::MouseRSkill, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::Skill1, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::Skill2, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::FarmingSkill, FOnSkillPressed());
	OnSkillPressedEvents.Add(ESkillType::UltimateSkill, FOnSkillPressed());

	// 기본 스킬 추가 => Skill InputID는 임의로 해놨습니다.

	// @아저씨 평타 딜링(마우스 좌클릭 Skill) 
	// @ 주먹으로 강한 스트레이트를 날린다 (오른쪽, 왼쪽 주먹 번갈아 나온다)
	FDSSkillSpec Skill1Spec = FDSSkillSpec(UDSTestSkill_1::StaticClass(), 1);
	SkillSpecHandles.Add(ESkillType::MouseLSkill ,GetSkillControlComponent()->AddSkill(Skill1Spec));
	DSEVENT_DELEGATE_BIND(OnSkillPressedEvents[ESkillType::MouseLSkill], this, &ADSCharacter_Girl::ActivateTestSkill1);

	// Cast<ADSPlayerController>(GetController())->GetPlayerInputComponent()
	// @ 소녀이동기 (R Skill)
	// FDSSkillSpec Skill2Spec = FDSSkillSpec(UDSTestSkill_2::StaticClass(), 2);
	// GetSkillControlComponent()->AddSkill(Skill2Spec);

	// @ 소녀 우클릭
	// FDSSkillSpec Skill3Spec = FDSSkillSpec(UDSTestSkill_3::StaticClass(), 3);
	// GetSkillControlComponent()->AddSkill(Skill3Spec);

	// @ 소년 RSkill
	// FDSSkillSpec Skill4Spec = FDSSkillSpec(UDSTestSkill_4::StaticClass(), 4);
	// GetSkillControlComponent()->AddSkill(Skill4Spec);
}

void ADSCharacter_Girl::Reset()
{
	DSEVENT_DELEGATE_REMOVE(OnSkillPressedEvents[ESkillType::MouseLSkill], this);
}

void ADSCharacter_Girl::ActivateTestSkill1()
{
	DS_NETLOG(DSSkillLog, Log, TEXT(""));

	if (false == HasAuthority()) 
	{
		if (UDSBaseAnimInstance* AnimInstance = Cast<UDSBaseAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			AnimInstance->PlayPunchMontage();
		}
	}

	ServerRPC_ActivateTestSkill1();
}

void ADSCharacter_Girl::ServerRPC_ActivateTestSkill1_Implementation()
{
	if (UDSBaseAnimInstance* AnimInstance = Cast<UDSBaseAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->PlayPunchMontage();
	}

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ADSCharacter_Girl* OtherPlayer = Cast<ADSCharacter_Girl>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPC_PlaySkillAnimation(this);
				}
			}
		}
	}
}

void ADSCharacter_Girl::ClientRPC_PlaySkillAnimation_Implementation(ADSCharacter_Girl* Player)
{
	check(Player);
	if (UDSBaseAnimInstance* AnimInstance = Cast<UDSBaseAnimInstance>(Player->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->PlayPunchMontage();
	}
}

void ADSCharacter_Girl::MulticastRPC_PlaySkillAnimation_Implementation()
{
	if (UDSBaseAnimInstance* AnimInstance = Cast<UDSBaseAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->PlayPunchMontage();
	}
}

void ADSCharacter_Girl::ActivateTestSkill2()
{
}

void ADSCharacter_Girl::ActivateTestSkill3()
{
}
