#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"

//Game
#include "System/DSEnums.h"
#include "Player/DSCheatManager.h"

//UHT
#include "DSPlayerInputComponent.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UDSInputConfig;
class ADSPlayerController;
class UDSInputComponent;

/** UDSPlayerInputComponent
 *
 * 플레이어 입력을 처리하고 InputAction과 GameplayTag를 매핑하는 Component
 */

DECLARE_MULTICAST_DELEGATE();

UCLASS()
class PROJECT25L_API UDSPlayerInputComponent : public UPawnComponent
{
	GENERATED_BODY()
	friend class UDSCheatManager;

public:
	UDSPlayerInputComponent(const FObjectInitializer& ObjectInitializer);
	
	void SetupInputComponent(UInputComponent* InputComponent);
	void InitialCharacterSetting();

	void SetCrounchMode(ECrouchMode TargetMode);

protected:
	// Move
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look_Mouse(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_StopJumping(const FInputActionValue& InputActionValue);
	void Input_Parkour(const FInputActionValue& InputActionValue);
	void Input_Sit(const FInputActionValue& InputActionValue);
	void Input_StopSit(const FInputActionValue& InputActionValue);
	void Input_ToggleSit(const FInputActionValue& InputActionValue);
	void Input_FastRun(const FInputActionValue& InputActionValue);
	void Input_StopFastRun(const FInputActionValue& InputActionValue);

	// System
	void Input_Pause(const FInputActionValue& InputActionValue);
	void Input_Interaction(const FInputActionValue& InputActionValue);

	// Skill
	void Input_Skill_1(const FInputActionValue& InputActionValue);
	void Input_Skill_2(const FInputActionValue& InputActionValue);
	void Input_Skill_Ultimate(const FInputActionValue& InputActionValue);
	void Input_Skill_Farming(const FInputActionValue& InputActionValue);

	// Item
	void Input_Item_QuickSlot1(const FInputActionValue& InputActionValue);
	void Input_Item_QuickSlot2(const FInputActionValue& InputActionValue);
	void Input_Item_QuickSlot3(const FInputActionValue& InputActionValue);
	void Input_Item_Vehicle(const FInputActionValue& InputActionValue);

	// Weapon
	void Input_Weapon_Toggle(const FInputActionValue& InputActionValue);
	void Input_Weapon_PrimaryAction_Started(const FInputActionValue& InputActionValue);
	void Input_Weapon_PrimaryAction_Onging(const FInputActionValue& InputActionValue);
	void Input_Weapon_PrimaryAction_Released(const FInputActionValue& InputActionValue);
	void Input_Weapon_SecondaryAction(const FInputActionValue& InputActionValue);

	// UI
	void Input_UI_Inventory(const FInputActionValue& InputActionValue);
	void Input_UI_Status(const FInputActionValue& InputActionValue);

	
	void SetSpeed(ESpeedType TargetwalkSpeed);

	ESpeedType CalculateSpeed();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Input")
	ESpeedType CurrentSpeedType;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDSInputComponent> DSInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Input")
	TObjectPtr<UDSInputConfig> InputConfig;

	UPROPERTY(EditAnywhere)
	uint8 bIsCrouched;

	UPROPERTY(EditAnywhere)
	uint8 bIsRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Mouse")
	float MouseSensitivity = 1.0f;

	float PressedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Mouse")
	float HoldTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECrouchMode CurrentCrouchMode = ECrouchMode::None;
};
