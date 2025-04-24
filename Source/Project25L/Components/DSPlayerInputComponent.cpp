// Default
#include "Components/DSPlayerInputComponent.h"

// UE
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"

//Game
#include "Animation/DSBaseAnimInstance.h"
#include "Character/Characters/DSCharacter.h"
#include "Character/Characters/DSCharacter_Girl.h"
#include "Components/DSCharacterMovementComponent.h"
#include "Components/DSFlightComponent.h"
#include "Components/DSInventoryComponent.h"
#include "Components/Skill/DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "GameData/GameplayTag/DSGameplayTags.h"
#include "HUD/DSHUD.h"
#include "Input/DSInputComponent.h"
#include "Item/DSItemActor.h"
#include "Player/DSPlayerController.h"
#include "System/DSEventSystems.h"
#include "System/DSGameUtils.h"
#include "System/DSUIManagerSubsystem.h"


UDSPlayerInputComponent::UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentSpeedType(ESpeedType::None)
	, DSInputComponent(nullptr)
	, bIsCrouched(false)
	, PressedStartTime(0.0f)
	, HoldTime(0.3f)
	, bIsInventoryMode(false)
	, MouseSensitivity(1.0f)
{
}

void UDSPlayerInputComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	SetInputMappingContext(EInputMappingContextType::DefaultIMC);

	DSInputComponent = Cast<UDSInputComponent>(InputComponent);

	if (IsValid(DSInputComponent))
	{
		SetCrounchMode(ECrouchMode::ToggleMode);

		// Move
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Move, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Move);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Look_Mouse);
		DSInputComponent->BindDualActions(InputConfig,	 FDSTags::GetDSTags().InputTag_Jump, this, &UDSPlayerInputComponent::Input_Jump, &UDSPlayerInputComponent::Input_StopJumping);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Parkour, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Parkour);
		DSInputComponent->BindDualActions(InputConfig,   FDSTags::GetDSTags().InputTag_FastRun, this, &UDSPlayerInputComponent::Input_FastRun, &UDSPlayerInputComponent::Input_StopFastRun);

		// System
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Pause, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Pause);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Interaction, ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Interaction);


		// Item
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot1, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot1);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot2, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot2);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_QuickSlot3, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_QuickSlot3);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Item_Vehicle, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Item_Vehicle);

		// Weapon
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Weapon_Toggle, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Weapon_Toggle);

		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Weapon_Equipment_toggle, ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Equipment_Toggle);

		// UI
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_UI_Inventory, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_UI_Inventory);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_UI_Status, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_UI_Status);

		// Skill
		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_PrimarySkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed );

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_SecondarySkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Skill1, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Skill2, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_UltimateSkill, this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		DSInputComponent->BindSkillActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_FarmingSkill , this,
			&UDSPlayerInputComponent::Input_Weapon_Attack_Started, &UDSPlayerInputComponent::Input_Weapon_Attack_Onging, &UDSPlayerInputComponent::Input_Weapon_Attack_Completed);

		// FlightSkill
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Begin, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skill_Flight_Begin);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Up, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Flight_Up);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Up, ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Skill_Flight_Released);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Down, ETriggerEvent::Triggered, this, &UDSPlayerInputComponent::Input_Skill_Flight_Down);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Down, ETriggerEvent::Completed, this, &UDSPlayerInputComponent::Input_Skill_Flight_Released);
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Dodge, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_Skill_Flight_Dodge);
		DSInputComponent->BindDualActions(InputConfig, FDSTags::GetDSTags().InputTag_Skill_Flight_Boost, this, &UDSPlayerInputComponent::Input_Skill_Flight_Boost, &UDSPlayerInputComponent::Input_Skill_Flight_Boost_Released);
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

	SetFlightDirection(MovementVector);
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
	ADSCharacter* Character = Cast<ADSCharacter>(PlayerController->GetPawn());
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
	//살쒔怨밴묶癒퐣 F袁ⓥ뀮筌
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

void UDSPlayerInputComponent::Input_Weapon_Attack_Started(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	//留뚯빟臾닿린瑜ㅺ퀬 덉 딅떎硫
	//臾닿린瑜좊떎.

	ADSCharacter* Character = GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		ADSArmedCharacter* ArmedCharacter = Cast<ADSArmedCharacter>(Character);

		if (IsValid(ArmedCharacter))
		{
			bool bIsEquipped = ArmedCharacter->GetIsEquipped();
			
			if (bIsEquipped == false)
			{
				ArmedCharacter->Equip();
			}
		}

		HoldTime = Character->GetInputThreshold();
	}

	UWorld* World = GetWorld();
	check(World);
	PressedStartTime = World->GetTimeSeconds();
}

void UDSPlayerInputComponent::Input_Weapon_Attack_Onging(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	FGameplayTag SkillTag =  ConvertInputTagToSkillTag(InputTag);
	if(SkillTag == FDSTags::GetDSTags().Skill_None)
	{	
		return;
	}
	DefaultAttack(SkillTag);
}

void UDSPlayerInputComponent::Input_Weapon_Attack_Completed(const FInputActionValue& InputActionValue, FGameplayTag InputTag)
{
	FGameplayTag SkillTag =  ConvertInputTagToSkillTag(InputTag);
	if(SkillTag == FDSTags::GetDSTags().Skill_None)
	{	
		return;
	}
	DefaultAttack(SkillTag);
}

void UDSPlayerInputComponent::DefaultAttack(FGameplayTag SkillTag)
{
	UWorld* World = GetWorld();

	check(World);
	float PressedTime = World->GetTimeSeconds() - PressedStartTime;

	if (PressedTime >= HoldTime)
	{
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
				if(true == SkillControlComponent->OnSkillPressedEvents.Contains(SkillTag))
				{
					DSEVENT_DELEGATE_INVOKE(SkillControlComponent->OnSkillPressedEvents[SkillTag]);
				}
			}
		}

		PressedStartTime = World->GetTimeSeconds();
	}
}

void UDSPlayerInputComponent::Input_Equipment_Toggle()
{
	ADSArmedCharacter* Character = GetPawn<ADSArmedCharacter>();
	check(Character);

	bool bIsEquipped = Character->GetIsEquipped();
	if (bIsEquipped)
	{
		//μ갑쒕떎.
		Character->UnEquip();
	}
	else
	{
		//μ갑댁젣쒕떎.
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

		if (bIsInventoryMode)
		{
			PlayerController->SetGameFocusMode();
			UIManager->PopContentToLayer(FDSTags::GetDSTags().UI_Layer_GameMenu_Inventory);
			bIsInventoryMode = false;
		}
		else
		{
			bIsInventoryMode = true;
			UIManager->PushContentToLayer(FDSTags::GetDSTags().UI_Layer_GameMenu_Inventory);
			PlayerController->SetUIFocusMode();
			// UI 낅뜲댄듃
		}
	}
}

void UDSPlayerInputComponent::Input_UI_Status(const FInputActionValue& InputActionValue)
{
}

void UDSPlayerInputComponent::Input_Skill_Flight_Begin(const FInputActionValue& InputActionValue)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();

	if (false == IsValid(FlightComponent))
	{
		return;
	}

	if (true == FlightComponent->EnableFlying())
	{
		DSEVENT_DELEGATE_INVOKE(OnInputMappingChangedEvent, EInputMappingContextType::FlightIMC);
		DSEVENT_DELEGATE_INVOKE(FlightComponent->OnFlightStateChanged, EFlightState::Begin);
	}
}

void UDSPlayerInputComponent::Input_Skill_Flight_Up(const FInputActionValue& InputActionValue)
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
		DSEVENT_DELEGATE_INVOKE(GetFlightComponent()->OnFlightStateChanged, EFlightState::Idle);
	}
}

void UDSPlayerInputComponent::Input_Skill_Flight_Down(const FInputActionValue& InputActionValue)
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
	
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (false == IsValid(Character))
	{
		return;
	}

}

void UDSPlayerInputComponent::Input_Skill_Flight_Dodge(const FInputActionValue& InputActionValue)
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

void UDSPlayerInputComponent::Input_Skill_Flight_Boost(const FInputActionValue& InputActionValue)
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

void UDSPlayerInputComponent::Input_Skill_Flight_Boost_Released(const FInputActionValue& InputActionValue)
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

	const UInputAction* IA = InputConfig->FindNativeInputActionForTag(FDSTags::GetDSTags().InputTag_Sit);

	if (false ==  IsValid(IA))
	{
		return;
	}

	DSInputComponent->RemoveBinds(IA);	

	if (TargetMode == ECrouchMode::HoldMode )
	{
		DSInputComponent->BindDualActions(InputConfig, FDSTags::GetDSTags().InputTag_Sit, this, &UDSPlayerInputComponent::Input_Sit, &UDSPlayerInputComponent::Input_StopSit);
	}
	else if (TargetMode == ECrouchMode::ToggleMode)
	{
		DSInputComponent->BindSingleActions(InputConfig, FDSTags::GetDSTags().InputTag_Sit, ETriggerEvent::Started, this, &UDSPlayerInputComponent::Input_ToggleSit);
	}

	CurrentCrouchMode = TargetMode;
}

UDSFlightComponent* UDSPlayerInputComponent::GetFlightComponent() const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return nullptr;
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


void UDSPlayerInputComponent::SetFlightDirection(const FVector2D& MovementVector)
{
	UDSFlightComponent* FlightComponent = GetFlightComponent();
	
	if (false == IsValid(FlightComponent))
	{
		return;
	}
	
	if(false == FlightComponent->EnableFlying())
	{
		return;
	}
	
	APlayerController* PlayerController = GetController<APlayerController>();
	if (false == IsValid(PlayerController))
	{
		return;
	}
	
	ADSCharacter* Character = Cast<ADSCharacter>(UDSGameUtils::GetCharacter(PlayerController));
	if (false == IsValid(Character))
	{
		return;
	}

}

FGameplayTag UDSPlayerInputComponent::ConvertInputTagToSkillTag(const FGameplayTag& InputTag)
{
	static const FString InputPrefix = TEXT("InputTag.");

	FString InputTagName = InputTag.ToString();

	if (InputTagName.StartsWith(InputPrefix))
	{
		FString SkillTagName = InputTagName.RightChop(InputPrefix.Len());
		return FGameplayTag::RequestGameplayTag(FName(*SkillTagName));
	}

	return FDSTags::GetDSTags().Skill_None;
}