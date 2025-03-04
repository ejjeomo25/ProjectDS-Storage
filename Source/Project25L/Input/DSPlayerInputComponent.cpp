// Default
#include "Input/DSPlayerInputComponent.h"

// UE
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"

//Game
#include "Character/DSCharacter.h"
#include "Character/DSCharacterMovementComponent.h"
#include "DSLogChannels.h"
#include "Input/DSInputComponent.h"
#include "Player/DSPlayerController.h"
#include "Item/DSItemActor.h"
#include "System/DSEventSystems.h"
#include "Character/DSCharacter_Girl.h"

UDSPlayerInputComponent::UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UDSPlayerInputComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	APlayerController *PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	
	check(Subsystem);
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(DefaultIMC, 0);

	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		SetCrounchMode(ECrouchMode::ToggleMode);

		// Move
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Move")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Move);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Look.Mouse")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Look_Mouse);
		DSInputComponent->BindDualActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Jump")), this, &UDSPlayerInputComponent::Input_Jump, &UDSPlayerInputComponent::Input_StopJumping);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Parkour")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Parkour);
		DSInputComponent->BindDualActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.FastRun")), this, &UDSPlayerInputComponent::Input_FastRun, &UDSPlayerInputComponent::Input_StopFastRun);

		// System
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Pause")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Pause);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.Interaction")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Interaction);
		
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
	
		//UI
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.UI.Inventory")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_UI_Inventory);
		DSInputComponent->BindSingleActions(InputConfig, FGameplayTag::RequestGameplayTag(FName("InputTag.UI.Status")), ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_UI_Status);
	
	}

}

void UDSPlayerInputComponent::InitialCharacterSetting()
{
	bIsCrouched = false;
	
	SetSpeed(ESpeedType::Forward);
	
	
	
}

void UDSPlayerInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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

	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	SetSpeed(ESpeedType::Backward);
}

void UDSPlayerInputComponent::Input_Pause(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Interaction(const FInputActionValue& InputActionValue)
{
	//오버랩 된 상태에서 F키를 누르면 
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}
	
	ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
	if (IsValid(Character))
	{
		ADSItemActor* Item = Cast< ADSItemActor>(Character->GetSurroundingItem());

		if (IsValid(Item))
		{
			int ItemID = Item->GetID();

			Item->SetLifeSpan(0.5f);
			Character->SetSurroundingItem(nullptr);
		}
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
	UWorld* World = GetWorld();
	check(World);
	PressedTime = World->GetTimeSeconds();
}

void UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Onging(const FInputActionValue& InputActionValue)
{
	UWorld* World = GetWorld();
	check(World);

	float CurrentTime = World->GetTimeSeconds();
	float HoldDuration = CurrentTime - PressedTime;

	if (HoldDuration >= HoldTime)
	{
		// Basic Weapon: Rapid Fire
		// Throwable Weapon: Aiming the trajectory
	}
	
}

void UDSPlayerInputComponent::Input_Weapon_PrimaryAction_Released(const FInputActionValue& InputActionValue)
{
	UWorld* World = GetWorld();
	check(World);

	float ReleasedTime = World->GetTimeSeconds();
	float ClickDuration = ReleasedTime - PressedTime;
	if (ClickDuration < HoldTime)
	{
		// Basic Weapon: Normal Attack 
		if (ADSCharacter_Girl* Player = Cast<ADSCharacter_Girl>(Cast<APlayerController>(GetOwner())->GetPawn()))
		{
			DSEVENT_DELEGATE_INVOKE(Player->OnSkillPressedEvents[ESkillType::MouseLSkill]);
		}
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("Hold"));
		// Basic Weapon: Rapid Fire Stop
		// Throwable Weapon: Attack at the targeted location
	}
}


void UDSPlayerInputComponent::Input_Weapon_SecondaryAction(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_UI_Inventory(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_UI_Status(const FInputActionValue& InputActionValue)
{
}



void UDSPlayerInputComponent::SetSpeed(ESpeedType TargetwalkSpeed)
{
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
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
	APlayerController* PlayerController = Cast<ADSPlayerController>(GetOwner());
	if (!IsValid(PlayerController))
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

	if (!IsValid(IA))
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
	else
	{
		// DS_LOG(DSLog, Log, TEXT("CurrentCrouchMode is None"));
	}

	CurrentCrouchMode = TargetMode;

	// DS_LOG(DSLog, Log, TEXT("CurrentCrouchMode: %s"), *StaticEnum<ECrouchMode>()->GetNameStringByValue(static_cast<int64>(CurrentCrouchMode)));
}


