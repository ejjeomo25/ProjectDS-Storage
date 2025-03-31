// Default
#include "Components/DSPlayerInputComponent.h"

// UE
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"

//Game
#include "Character/Characters/DSCharacter.h"
#include "Character/Characters/DSCharacter_Girl.h"
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "DSLogChannels.h"
#include "HUD/DSHUD.h"
#include "Input/DSInputComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"


UDSPlayerInputComponent::UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentSpeedType(ESpeedType::None)
	, bIsCrouched(false)
	, HoldTime(0.3f)
	, bIsInventoryMode(false)
{
}

void UDSPlayerInputComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	SetInputMappingContext(EInputMappingContextType::DefaultIMC);

	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		SetCrounchMode(ECrouchMode::ToggleMode);

		//*****************코드 리뷰 : 태그 분리 ****************************//
		// Move
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Move")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Move);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Look.Mouse")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Look_Mouse);
		DSInputComponent->BindDualActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Jump")), this, &UDSPlayerInputComponent::Input_Jump, &UDSPlayerInputComponent::Input_StopJumping);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Parkour")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Parkour);
		DSInputComponent->BindDualActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.FastRun")), this, &UDSPlayerInputComponent::Input_FastRun, &UDSPlayerInputComponent::Input_StopFastRun);

		// System
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Pause")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Pause);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Interaction")), ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Interaction);
		
		// Skill
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.1")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_1);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.2")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_2);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Ultimate")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Ultimate);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Farming")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Farming);
	
		// Item
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Item.QuickSlot1")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot1);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Item.QuickSlot2")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot2);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Item.QuickSlot3")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot3);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Item.Vehicle")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_Vehicle);

		// Weapon
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Weapon.Toggle")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Weapon_Toggle);
		DSInputComponent->BindTripleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Weapon.PrimaryAction")), this,
			&UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Started, &UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Onging, &UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Released);
		
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Weapon.SecondaryAction")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Weapon_SecondaryAction);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Weapon.Equipment_toggle")), ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Equipment_Toggle);

		//UI
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.UI.Inventory")), ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_UI_Inventory);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.UI.Status")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_UI_Status);


		// FlightSkill
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Begin")), ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skil_Flight_Begin);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Up")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skil_Flight_Up);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Up")), ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Skill_Flight_Released);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Down")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skil_Flight_Down);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Down")), ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Skill_Flight_Released);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Dodge")), ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skil_Flight_Dodge);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Skill.Flight.Boost")), ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skil_Flight_Boost);
	}

	DSEVENT_DELEGATE_BIND(OnInputMappingChangedEvent, this, &UDSPlayerInputComponent::SetInputMappingContext);
}

void UDSPlayerInputComponent::SetInputMappingContext(EInputMappingContextType NewIMCType)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController) || false == IsValid(InputMappingContexts[NewIMCType]))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	check(Subsystem);
	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMappingContexts[NewIMCType], 0);
}

void UDSPlayerInputComponent::InitialCharacterSetting()
{
	bIsCrouched = false;
	bIsInventoryMode = false;
	SetSpeed(ESpeedType::Forward);
}

void UDSPlayerInputComponent::OnUnregister()
{
	DSEVENT_DELEGATE_REMOVE(OnInputMappingChangedEvent, this);

	Super::OnUnregister();
}

void UDSPlayerInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	APlayerController* PlayerController = GetController<APlayerController>();
	
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}

	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ESpeedType NewSpeedType = CalculateSpeed();
	if (CurrentSpeedType != NewSpeedType)
	{
		SetSpeed(NewSpeedType);
		CurrentSpeedType = NewSpeedType;
	}
	
	Pawn = PlayerController->GetPawn();
	Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	Pawn->AddMovementInput(RightDirection, MovementVector.X);
}


void UDSPlayerInputComponent::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	APlayerController* PlayerController = GetController<APlayerController>();


	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * MouseSensitivity);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(-Value.Y * MouseSensitivity);
	}
}

void UDSPlayerInputComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		if (PlayerController->IsInputKeyDown(EKeys::W))
		{
			bIsRun = true;
			Character->SetJumpHeight(bIsRun);
		}
		Character->Jump();
		bIsRun = false;
	}
}

void UDSPlayerInputComponent::Input_StopJumping(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		Character->StopJumping();
		Character->SetJumpHeight(bIsRun);
	}
}

void UDSPlayerInputComponent::Input_Parkour(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Sit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		Character->Crouch();
	}
}

void UDSPlayerInputComponent::Input_StopSit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		Character->UnCrouch();
	}
}

void UDSPlayerInputComponent::Input_ToggleSit(const FInputActionValue& InputActionValue)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false == IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		if (bIsCrouched)
		{
			Character->UnCrouch(); 
			bIsCrouched = false;
		}
		else
		{
			Character->Crouch();
			bIsCrouched = true;
		}
	}

}

void UDSPlayerInputComponent::Input_FastRun(const FInputActionValue& InputActionValue)
{
	SetSpeed(ESpeedType::Sprint);
}

void UDSPlayerInputComponent::Input_StopFastRun(const FInputActionValue& InputActionValue)
{
	SetSpeed(ESpeedType::Forward);
}

void UDSPlayerInputComponent::Input_Pause(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Interaction(const FInputActionValue& InputActionValue)
{
	//오버랩 된 상태에서 F키를 누르면 

	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (IsValid(Character))
	{
		Character->TryInteraction();
	}
}

void UDSPlayerInputComponent::Input_Skill_1(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skill_2(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skill_Ultimate(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skill_Farming(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_QuickSlot1(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_QuickSlot2(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_QuickSlot3(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Item_Vehicle(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Weapon_Toggle(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Started(const FInputActionValue& InputActionValue)
{
	//실행할 때 HoldTime 업데이트 한다. 무기의 Threshold값으로
	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		HoldTime = Character->GetInputThreshold();
	}

	UWorld* World = GetWorld();
	check(World);
	PressedTime = World->GetTimeSeconds();
}

void UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Onging(const FInputActionValue& InputActionValue)
{
	UWorld* World = GetWorld();

	check(World);
	PressedTime += World->GetDeltaSeconds();

	DefaultAttack();
}

void UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Released(const FInputActionValue& InputActionValue)
{
	DefaultAttack();
}

void UDSPlayerInputComponent::DefaultAttack()
{

	if (PressedTime >= HoldTime)
	{
		PressedTime = 0.f;

		APlayerController* PlayerController = GetController<APlayerController>();

		if (false == IsValid(PlayerController))
		{
			return;
		}

		ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
		if (IsValid(Character))
		{
			UDSSkillControlComponent* SkillControlComponent = Character->GetSkillControlComponent();
			if (IsValid(SkillControlComponent))
			{
				if(true == SkillControlComponent->OnSkillPressedEvents.Contains(ESkillType::PrimarySkill))
				{
					DSEVENT_DELEGATE_INVOKE(SkillControlComponent->OnSkillPressedEvents[ESkillType::PrimarySkill]);
				}
			}
		}
	}
}

UDSFlightComponent* UDSPlayerInputComponent::GetFlightComponent() const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Hold"));
	}

	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (false == IsValid(Character))
	{
		return nullptr;
	}

	UDSFlightComponent* FlightComponent = Character->GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return nullptr;
	}

	return FlightComponent;
}

void UDSPlayerInputComponent::Input_Weapon_SecondaryAction(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Equipment_Toggle()
{
	ADSArmedCharacter* Character = GetPawn<ADSArmedCharacter>();
	
	check(Character);

	bool bIsEquipped = Character->GetIsEquipped();
	if (bIsEquipped)
	{
		//장착된 상태-> 해제한다.
		Character->UnEquip();
	}
	else
	{
		//해제된 상태->장착한다.
		Character->Equip();
	}

}

void UDSPlayerInputComponent::Input_UI_Inventory(const FInputActionValue& InputActionValue)
{
	DS_NETLOG(DSNetLog, Log, TEXT("Inventory"));

	ADSPlayerController* PlayerController = GetController<ADSPlayerController>();
	if (IsValid(PlayerController))
	{
		UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
		check(UIManager);


		//*****************코드 리뷰 : 태그 분리 ****************************//
		if (bIsInventoryMode)
		{
			PlayerController->SetGameFocusMode();
			UIManager->PopContentToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu.Inventory")));
			bIsInventoryMode = false;
		}
		else
		{
			bIsInventoryMode = true;
			UIManager->PushContentToLayer(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu.Inventory")));
			PlayerController->SetUIFocusMode();
		}

	}
}

void UDSPlayerInputComponent::Input_UI_Status(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skil_Flight_Begin(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	// 비행 가능한 상태라면
	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(OnInputMappingChangedEvent, EInputMappingContextType::FlightIMC);
		DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Begin);
	}
}

void UDSPlayerInputComponent::Input_Skil_Flight_Up(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Up);
	}
}

void UDSPlayerInputComponent::Input_Skill_Flight_Released(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(GetFlightComponent()->OnFlightStateChanged, EFlightState::Hovering);
	}
}

void UDSPlayerInputComponent::Input_Skil_Flight_Down(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(GetFlightComponent()->OnFlightStateChanged, EFlightState::Down);
	}
}

void UDSPlayerInputComponent::Input_Skil_Flight_Dodge(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(GetFlightComponent()->OnFlightStateChanged, EFlightState::Dodge);
	}
}

void UDSPlayerInputComponent::Input_Skil_Flight_Boost(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(GetFlightComponent()->OnFlightStateChanged, EFlightState::Boost);
	}
}



void UDSPlayerInputComponent::SetSpeed(ESpeedType TargetwalkSpeed)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false ==  IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (false ==  IsValid(Pawn))
	{
		return;
	}
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		UDSCharacterMovementComponent* DSMovemnt = Cast<UDSCharacterMovementComponent>(Character->GetCharacterMovement());
		if (DSMovemnt)
		{
			DSMovemnt->SetSpeedCommand(TargetwalkSpeed);
		}
		
	}
}

ESpeedType UDSPlayerInputComponent::CalculateSpeed()
{

	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return ESpeedType::None;
	}
	bool bPressedW = PlayerController->IsInputKeyDown(EKeys::W);
	bool bPressedS = PlayerController->IsInputKeyDown(EKeys::S);
	bool bPressedA = PlayerController->IsInputKeyDown(EKeys::A);
	bool bPressedD = PlayerController->IsInputKeyDown(EKeys::D);
	bool bPressedShift = PlayerController->IsInputKeyDown(EKeys::LeftShift);
	
	if ((bPressedW && bPressedS) || (bPressedA && bPressedD))
	{
		return ESpeedType::None;
	}

	if (bPressedShift)
	{
		return ESpeedType::Sprint;
	}

	if (bPressedS)
	{
		return ESpeedType::Backward;
	}
	
	return ESpeedType::Forward;
}


void UDSPlayerInputComponent::SetCrounchMode(ECrouchMode TargetMode)
{
	if (CurrentCrouchMode == TargetMode)
	{
		return;
	}

	const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FGameplayTag::RequestGameplayTag(FName("InputTag.Sit")));

	if (false ==  IsValid(IA))
	{
		return;
	}

	DSInputComponent->RemoveBinds(IA);	

	if (TargetMode == ECrouchMode::HoldMode )
	{
		DSInputComponent->BindDualActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Sit")), this, &UDSPlayerInputComponent::Input_Sit, &UDSPlayerInputComponent::Input_StopSit);
	}
	else if (TargetMode == ECrouchMode::ToggleMode)
	{
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Sit")), ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_ToggleSit);
	}

	CurrentCrouchMode = TargetMode;
}


