//Default
#include "Character/DSCharacter.h"

//UE
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

//Game
#include "DSCharacterMovementComponent.h"
#include "DSLogChannels.h"
#include "Inventory/DSInventoryComponent.h"
#include "Skill/DSSkillControlComponent.h"
#include "Skill/DSSkillSpec.h"
#include "Skill/DSTestSkill.h"
#include "System/DSEventSystems.h"


ADSCharacter::ADSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->bUsePawnControlRotation = false; //QuaterView는 회전하지 않는다.
	CameraSpringArm->TargetArmLength = 500.f;
	CameraSpringArm->SetUsingAbsoluteRotation(true);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	CameraSpringArm->bUsePawnControlRotation = true;

	bIsCrouched = true;

	InventoryComponent = CreateDefaultSubobject<UDSInventoryComponent>(TEXT("InventoryComponent"));
}

void ADSCharacter::AddSkill(const int32 InputID)
{
	FDSSkillSpec NewSkillSpec(UDSTestSkill::StaticClass(), InputID);
	GetSkillControlComponent()->AddSkill(NewSkillSpec);
}

void ADSCharacter::SetSurroundingItem(AActor* Actor)
{
	//거리 계산을 통해서 현재 SurroundingItem 보다 거리가 가까운 액터를 설정한다.
	if (SurroundingItem.IsValid())
	{
		if (IsValid(Actor) == false)
		{
			//리셋의 경우
			SurroundingItem = nullptr;
			return;
		}

		double Distance1 = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
		double Distance2 = FVector::Dist(GetActorLocation(), SurroundingItem->GetActorLocation());

		DS_LOG(DSItemLog, Log, TEXT("Character - Item Distance %lf"),Distance1);
		DS_LOG(DSItemLog, Log, TEXT("Character - Surrounding Item Distance %lf"), Distance2);

		if (Distance2 > Distance1)
		{
			SurroundingItem = Actor;
		}
	}
	else
	{
		SurroundingItem = Actor;
	}
}

void ADSCharacter::MulticastRPC_SetGimmickState_Implementation(bool bShouldChange)
{
	// 기믹의 상태가 변경되었음을 전달
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnGimmickStateChanged, bShouldChange);
}

void ADSCharacter::ServerRPC_UseItem_Implementation(int32 ItemID, int32 ItemCount)
{
	for (int ItemIdx = 0; ItemIdx < ItemCount; ItemIdx++)
	{
		DS_LOG(DSItemLog, Log, TEXT("ItemID %d ItemCount %d"), ItemID, ItemCount);
		InventoryComponent->OnItemUsed(ItemID);
	}
}
