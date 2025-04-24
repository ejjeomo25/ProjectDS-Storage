//Default
#include "Character/Characters/DSCharacter.h"

//UE
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"

//Game
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Components/DSPlayerInputComponent.h"
#include "Components/DSStatComponent.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "Game/DSGameState.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "GameData/Items/DSItemData.h"
#include "HUD/DSHUD.h"
#include "Item/DSGiftBox.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "Skill/Base/DSTestSkill.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Game/DSMainWidget.h"
#include "UI/Game/Player/DS_HPBar.h"
#include "UI/Game/Player/DSPlayerInfo.h"
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

void ADSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeStat();
}

void ADSCharacter::InitializeStat()
{
	ADSPlayerController* PlayerController = GetController<ADSPlayerController>();

	if (IsValid(PlayerController))
	{
		ECharacterType CharacterType = PlayerController->GetCharacterType();
		
		StatComponent->InitializeStats(EDataTableType::CharacterData, CharacterType, -1);
	}
}

void ADSCharacter::InitializeUI()
{
	if (false == IsLocallyControlled())
	{
		return;
	}

	//현재 로컬 환경에서
	//Pawn을 모두 가져온다.
	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);

	check(UIManager);

	UDSMainWidget* MainWidget = Cast<UDSMainWidget>(UIManager->GetTopLayer(FDSTags::GetDSTags().UI_Layer_Game));

	if (false == IsValid(MainWidget))
	{
		return;
	}

	//자기 자신 초기화
	BindWidget(StatComponent, MainWidget, 0);

	UWorld* World = GetWorld();

	check(World);

	AGameStateBase* GameState = World->GetGameState();

	int32 PlayerIndex = 1;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		ADSCharacter* Character = PS->GetPawn<ADSCharacter>();

		if (this == Character)
		{
			//나 자신과 같은 경우는 이미 연결했기 때문에 제외한다.
			continue;
		}
		if (IsValid(Character))
		{
			UDSStatComponent* TargetStatComponent = Character->StatComponent;
			if (IsValid(TargetStatComponent))
			{
				BindWidget(TargetStatComponent, MainWidget, PlayerIndex++);
			}
		}
	}
}

void ADSCharacter::BindWidget(UDSStatComponent* InStatComponent, UDSMainWidget* MainWidget, int32 PlayerIndex)
{
	if (false == IsValid(InStatComponent) || false == IsValid(MainWidget))
	{
		return;
	}

	UDS_HPBar* HPBar = MainWidget->GetHPBar(PlayerIndex);
	UDSPlayerInfo* PlayerInfo = MainWidget->GetPlayerInfo(PlayerIndex);
	if (IsValid(HPBar) && IsValid(PlayerInfo))
	{
		if (IsValid(InStatComponent))
		{
			/*초기화 => StatComponent가 이미 초기화 된 이후에 이 함수가 호출되어진다.*/
			DSEVENT_DELEGATE_BIND(InStatComponent->OnHPChanged, HPBar, &UDS_HPBar::SetHP);
			DSEVENT_DELEGATE_BIND(InStatComponent->OnHPChanged, PlayerInfo, &UDSPlayerInfo::SetText);

			//초기화 해준다.
			DSEVENT_DELEGATE_INVOKE(InStatComponent->OnHPChanged, InStatComponent->GetCurrentHP(), InStatComponent->GetMaxHP());
		}
	}
}

void ADSCharacter::ServerRPC_ReadyPlayer_Implementation(bool bCanClose, int32 PlayerCount,const FGameplayTag& ReadyPlayerWidgetTag)
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
				Character->ClientRPC_ReadyPlayer(bCanClose, PlayerCount, ReadyPlayerWidgetTag);
			}
		}
	}

}

void ADSCharacter::ClientRPC_ReadyPlayer_Implementation(bool bCanClose, int32 PlayerCount, const FGameplayTag& ReadyPlayerWidgetTag)
{
	UWorld* World = GetWorld();

	check(World);

	ADSGameState* GameState = Cast<ADSGameState>(World->GetGameState());

	if (false == IsValid(GameState))
	{
		return;
	}

	int32 MaxPlayerCount = GameState->GetMaxPlayerCount();

	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
	check(UIManager);

	if (false == bCanClose)
	{
		UIManager->PushContentToLayer(ReadyPlayerWidgetTag);
	}
	else
	{
		//false로 변경되어 질때 UI를 없앤다.
		UIManager->PopContentToLayer(ReadyPlayerWidgetTag);
	}


	//플레이어가 n명일 때
	UDSReadyPlayersWidget* WidgetLayer = Cast<UDSReadyPlayersWidget>(UIManager->GetTopLayer(FDSTags::GetDSTags().UI_Layer_Modal));

	if (IsValid(WidgetLayer))
	{
		WidgetLayer->UpdatePlayerCount(PlayerCount, MaxPlayerCount);
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
	FDSSkillSpec NewSkillSpec(UDSTestSkill::StaticClass(), ESkillType::TestSkill, FDSTags::GetDSTags().Skill_TestSkill, InputID);
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


void ADSCharacter::ServerRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	//모든 클라이언트를 가지고 와서, 현재 클라이언트의 HeldItem과 같을 경우에 제거해주어야한다.
	//클라이언트RPC를 쏴야함.

	if (false == HeldItem.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (IsValid(PlayerController))
		{
			ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

			if (IsValid(Character))
			{
				//현재 서버의 HeldItem과 같다면, UI를 반영해야함.

				if (Character->HeldItem.IsValid())
				{
					if (Character->HeldItem == HeldItem)
					{
						Character->ClientRPC_RemoveItemData(IndexToRemove);
					}
				}
			}
		}
	}

	// 서버에서 데이터 업데이트
	HeldItem->RemoveItemData(IndexToRemove);
}

void ADSCharacter::ClientRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	//UDSGiftBox 를 가지고온다.
	if (HeldItem.IsValid())
	{
		if (IsLocallyControlled())
		{
			// 클라에서 UI업데이트
			HeldItem->RemoveItemUI(IndexToRemove);
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
			
			bool bIsItemStored = InventoryComponent->StoreItems(nullptr, ItemInfo);

			if (bIsItemStored)
			{
				ServerRPC_RemoveItemData(ItemIdx);
			}
		}
	}
}

void ADSCharacter::PickupItem(AActor* Interactor)
{
	ADSItemActor* SurroundingItem = Cast<ADSItemActor>(Interactor);

	if (IsValid(SurroundingItem))
	{
		TArray<FDSItemInfo> ItemData = SurroundingItem->GetItemData();

		bool bIsItemStored = InventoryComponent->StoreItems(SurroundingItem, ItemData[0]);
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
		/*서버랑 클라이언트를 설정해두어야지, UI를 사용해서 인터랙션을 할 때 RPC 전송이 가능하다.*/
		/*SelectedItem은 클라이언트만 작동한다. => 그래서 서버에서 작동할 수 있도록 서버도 아이템으로 덮는다.*/
		//주변 아이템을 설정해준다, 클라이언트 설정
		HeldItem = SurroundingItem;
		
		ServerRPC_SetSurroundingItem(SurroundingItem);

		TArray<FDSItemInfo> ItemInfo = SurroundingItem->GetItemData();
		DSEVENT_DELEGATE_INVOKE(SurroundingItem->OnUpdateItemWidget, ItemInfo);
	}
}

void ADSCharacter::ServerRPC_SetSurroundingItem_Implementation(ADSGiftBox* Interactor)
{
	if (IsValid(Interactor))
	{
		//SurroundingItem이 실제 있다면,
		HeldItem = Interactor;
	}
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
