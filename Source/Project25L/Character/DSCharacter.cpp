//Default
#include "Character/DSCharacter.h"

//UE
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagContainer.h"

//Game
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"
#include "DSLogChannels.h"

#include "DSCharacterMovementComponent.h"
#include "DSFlightComponent.h"
#include "Input/DSPlayerInputComponent.h"

#include "GameData/Items/DSItemData.h"

#include "Inventory/DSInventoryComponent.h"
#include "Item/DSGiftBox.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"

#include "Skill/DSSkillControlComponent.h"
#include "Skill/DSTestSkill.h"

#include "UI/HUB/DSReadyPlayersWidget.h"
#include "HUD/DSHUD.h"

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

	bIsCrouched = true;
	bIsShowGiftBox = false;

	InventoryComponent = CreateDefaultSubobject<UDSInventoryComponent>(TEXT("InventoryComponent"));
	DSPlayerInputComponent = CreateDefaultSubobject<UDSPlayerInputComponent>(TEXT("DSPlayerInputComponent"));
	FlightComponent = CreateDefaultSubobject<UDSFlightComponent>(TEXT("FlightComponent"));

	SetJumpHeight(false);
}

void ADSCharacter::AddSkill(const int32 InputID)
{
	FDSSkillSpec NewSkillSpec(UDSTestSkill::StaticClass(), InputID);
	GetSkillControlComponent()->AddSkill(NewSkillSpec);
}

FVector ADSCharacter::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForwardVector() * CameraSpringArm->TargetArmLength;
}

FVector ADSCharacter::GetCameraForwardVector()
{
	return  Camera->GetForwardVector();
}

float ADSCharacter::GetFOV()
{
	return FOV;
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

		ADSPlayerController* PlayerController = Cast<ADSPlayerController>(GetController());

		if (IsValid(PlayerController) == false)
		{
			return;
		}

		if (PlayerCount <= 1)
		{
			// 기믹의 상태가 변경되었음을 전달
			UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
			check(UIManager);


			// 한명일 때
			if (PlayerCount==1)
			{
				//true로 변경되어질 때 UI를 띄운다.
				UIManager->PushContentToLayer(PlayerController, ReadyPlayerWidgetTag);
			}
			else
			{
				//false로 변경되어 질때 UI를 없앤다.
				UIManager->PopContentToLayer(PlayerController, ReadyPlayerWidgetTag);
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
void ADSCharacter::ServerRPC_UseItem_Implementation(int32 ItemID, int32 ItemCount)
{
	for (int ItemIdx = 0; ItemIdx < ItemCount; ItemIdx++)
	{
		InventoryComponent->UseItem(ItemID);
	}
}

void ADSCharacter::ServerRPC_PrintItem_Implementation()
{
	InventoryComponent->PrintItem();
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

			// DSEVENT_DELEGATE_INVOKE(GameUI.OnUpdateChestWidget, ItemData);
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
		bIsShowGiftBox = true;
		// DSEVENT_DELEGATE_INVOKE(GameUI.OnUpdateChestWidget, ItemInfo);
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


void ADSCharacter::SetJumpHeight(uint8  bIsRun)
{
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