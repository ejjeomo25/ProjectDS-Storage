#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameplayTagContainer.h"

//Game
#include "Character/DSCharacterBase.h"
#include "Components/DSStatComponent.h"
#include "Skill/Base/DSSkillSpec.h"

//UHT
#include "DSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDSInventoryComponent;
struct FDSSkillSpecHandle;
class ADSGiftBox;
class UDSPlayerInputComponent;
class UDSFlightComponent;

// Delegate
DECLARE_MULTICAST_DELEGATE(FOnInventoryToggle);

UCLASS()
class PROJECT25L_API ADSCharacter : public ADSCharacterBase
{
	GENERATED_BODY()
	
public:

	ADSCharacter(const FObjectInitializer& ObjectInitializer);


/*Getter/Setter 함수*/
public:
	FORCEINLINE FVector GetCameraForwardVector();
	FORCEINLINE float GetFOV() { return FOV; }
	FORCEINLINE UDSFlightComponent* GetFlightComponent() const { return FlightComponent; }
	FORCEINLINE UDSPlayerInputComponent* GetPlayerInputComponent() const { return DSPlayerInputComponent; }
	FORCEINLINE virtual float GetInputThreshold();
	FORCEINLINE UDSInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE FVector CalPlayerLocalCameraStartPos();
public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReadyPlayer(int32 PlayerCount, FGameplayTag ReadyPlayerWidgetTag);

protected:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReadyPlayer(int32 PlayerCount, FGameplayTag ReadyPlayerWidgetTag);

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_UseItem(int32 ItemID, int32 ItemCount);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_PrintItem();

	void AddSkill(const int32 InputID);
	void PickupItem(AActor* Interactor);
	void SelectedItem(AActor* Interactor);

public:
/*Item*/

	void TryPickupItem(int32 ItemIdx);
	/*각 로컬에서만 동작하는 아이템 획득 시도하는 함수*/
	void TryInteraction();

public:
/*UI*/
	FOnInventoryToggle OnInventoryToggle;
	
public:
	virtual float TakeFinalDamage(float DamageAmount, const FDSDamageEvent& NewDamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected:
/*Input*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

public:
	void SetJumpHeight(bool bIsRun);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_SetJumpVelocity(bool bIsRun);
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<ADSGiftBox> HeldItem;

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	TObjectPtr<UDSInventoryComponent> InventoryComponent;

protected:
	/*Spring Arm, Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSFlightComponent> FlightComponent;

protected:
	/*Character Setting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Jump")
	float NomalJumpHeight = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Jump")
	float RunJumpHeight = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Farming")
	float FarmingRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|FOV")
	float FOV;


protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSPlayerInputComponent> DSPlayerInputComponent;
};
