//Default
#include "Character/Characters/DSCharacter.h"

//UE
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"

//Game
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "DSLogChannels.h"
#include "GameData/Items/DSItemData.h"
#include "HUD/DSHUD.h"
#include "Components/DSPlayerInputComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Item/DSGiftBox.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "Skill/Base/DSTestSkill.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/HUB/DSReadyPlayersWidget.h"

ADSCharacter::ADSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
	, HeldItem(nullptr)
	, FarmingRadius(100.f)

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

	InventoryComponent = CreateDefaultSubobject<UDSInventoryComponent>(TEXT("InventoryComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent>(TEXT("DSPlayerInputComponent"));
	FlightComponent = CreateDefaultSubobject<UDSFlightComponent>(TEXT("FlightComponent"));

	SetJumpHeight(false);
}

FVector ADSCharacter::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForwardVector() * CameraSpringArm->TargetArmLength;
}

FVector ADSCharacter::GetCameraForwardVector()
{
	return  Camera->GetForwardVector();
}

float ADSCharacter::GetInputThreshold()
{
	//아저씨의 경우 알아서 정의해서 여기서 리턴.
	return 0.0f;
}

void ADSCharacter::ServerRPC_ReadyPlayer_Implementation(int32 PlayerCount, FGameplayTag ReadyPlayerWidgetTag)
{
	//모든 클라이언트를 가져온다.
	UWorld* World = GetWorld();
	check(World);

	for (auto Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (IsValid(PlayerController))
		{
			// 로직
			ADSCharacter* Character = Cast<ADSCharacter>(PlayerController->GetCharacter());

			if (IsValid(Character))
			{
				Character->ClientRPC_ReadyPlayer(PlayerCount, ReadyPlayerWidgetTag);
			}
		}
	}

}

void ADSCharacter::ClientRPC_ReadyPlayer_Implementation(int32 PlayerCount, FGameplayTag ReadyPlayerWidgetTag)
{
	if (IsLocallyControlled())
	{
		if (PlayerCount <= 1)
		{
			// 기믹의 상태가 변경되었음을 전달
			UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
			check(UIManager);


			// 한명일 때
			if (PlayerCount==1)
			{
				//true로 변경되어질 때 UI를 띄운다.
				UIManager->PushContentToLayer(ReadyPlayerWidgetTag);
			}
			else
			{
				//false로 변경되어 질때 UI를 없앤다.
				UIManager->PopContentToLayer(ReadyPlayerWidgetTag);
			}
		}
		else
		{
			//플레이어가 n명일 때
			UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
			check(UIManager);
		}

	}
}

//Cheat 
void ADSCharacter::ServerRPC_UseItem_Implementation(int32 ItemID, int32 ItemCount)
{
	for (int ItemIdx = 0; ItemIdx < ItemCount; ItemIdx++)
	{
		if (IsValid(InputComponent))
		{
			InventoryComponent->UseItem(ItemID);
		}
	}
}

void ADSCharacter::ServerRPC_PrintItem_Implementation()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->PrintItem();
	}
}

void ADSCharacter::AddSkill(const int32 InputID)
{
	FDSSkillSpec NewSkillSpec(UDSTestSkill::StaticClass(), ESkillType::None, InputID);
	GetSkillControlComponent()->AddSkill(NewSkillSpec);
}

void ADSCharacter::TryInteraction()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility)); /*아이템 채널로 변경 해야 함!!*/
	IgnoreActors.Add(this);

	FVector Location = GetActorLocation();

	bool Result = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Location, FarmingRadius, ObjectTypes, nullptr, IgnoreActors, OutActors);
	float MinDistance = FLT_MAX;

	if (Result)
	{

		ADSItemActor* SurroundingItem = nullptr;
		for (AActor* Actor : OutActors)
		{
			//가장 가까운 아이템을 섭치한다.
			//시야 각도에 들어오는지 확인한다.
			ADSItemActor* TmpItem = Cast<ADSItemActor>(Actor);

			if (IsValid(TmpItem))
			{
				bool bIsInFOV = UDSGameUtils::IsWithinCharacterFOV(this, TmpItem, GetFOV());
				//시야각 범위 내에서
				if (bIsInFOV)
				{
					//거리가 가장 가까운 아이템을 줍는다.
					float Distance = FVector::Distance(Location, TmpItem->GetActorLocation());

					if (MinDistance > Distance)
					{
						MinDistance = Distance;
						SurroundingItem = TmpItem;
					}
				}
			}
		}

		if (IsValid(SurroundingItem))
		{
			EInteractType InteractionTyp = SurroundingItem->GetInteractType();

			switch (InteractionTyp)
			{
			case EInteractType::PickupItem:
				PickupItem(SurroundingItem);
				break;
			case EInteractType::SelectedItem:
				SelectedItem(SurroundingItem);
				break;
			}
		}
	}
}

void ADSCharacter::TryPickupItem(int32 ItemIdx)
{
	if (HeldItem.IsValid())
	{
		TArray<FDSItemInfo> ItemData = HeldItem->GetItemData();
		if (ItemData.IsValidIndex(ItemIdx))
		{
			FDSItemInfo ItemInfo = ItemData[ItemIdx];
			
			InventoryComponent->StoreItems(nullptr, ItemInfo.ID, 1);

			HeldItem->ServerRPC_RemoveItemData(ItemIdx);
		}
	}
}

void ADSCharacter::PickupItem(AActor* Interactor)
{
	ADSItemActor* SurroundingItem = Cast<ADSItemActor>(Interactor);

	if (IsValid(SurroundingItem))
	{
		InventoryComponent->StoreItems(SurroundingItem, -1, 1);
	}	
}

void ADSCharacter::SelectedItem(AActor* Interactor)
{
	ADSPlayerController* PlayerController = Cast<ADSPlayerController>(UDSGameUtils::GetPlayerController(this));
	
	if (IsValid(PlayerController))
	{
		PlayerController->SetUIFocusMode();
	}

	ADSGiftBox* SurroundingItem = Cast<ADSGiftBox>(Interactor);

	if (IsValid(SurroundingItem))
	{
		HeldItem = SurroundingItem;
		
		TArray<FDSItemInfo> ItemInfo = SurroundingItem->GetItemData();
		DSEVENT_DELEGATE_INVOKE(SurroundingItem->OnUpdateItemWidget, ItemInfo);
	}
}

float ADSCharacter::TakeFinalDamage(float DamageAmount, const FDSDamageEvent& NewDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(nullptr != GetStatComponent()) 
	{
		GetStatComponent()->ReceiveDamage(DamageAmount, NewDamageEvent.DamageType, NewDamageEvent.ElementType, this);
	}
	return Super::TakeDamage(DamageAmount, NewDamageEvent, EventInstigator, DamageCauser);
}

void ADSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsLocallyControlled())
	{
		DSPlayerInputComponent->SetupInputComponent(InputComponent);
	}
}

void ADSCharacter::SetJumpHeight(bool bIsRun)
{
	if (false == HasAuthority())
	{
		ServerRPC_SetJumpVelocity(bIsRun);
	}

	const float Gravity = 980.0f;
	float JumpVelocity = 0.f;

	if (bIsRun)
	{
		JumpVelocity = FMath::Sqrt(2 * Gravity * RunJumpHeight);
	}
	else
	{
		JumpVelocity = FMath::Sqrt(2 * Gravity * NomalJumpHeight);
	}
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;

}

void ADSCharacter::ServerRPC_SetJumpVelocity_Implementation(bool bIsRun)
{
	SetJumpHeight(bIsRun);
}
