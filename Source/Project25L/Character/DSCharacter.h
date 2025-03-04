#pragma once
//Default
#include "CoreMinimal.h"

//UE

//Game
#include "Character/DSCharacterBase.h"

//UHT
#include "DSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDSInventoryComponent;

UCLASS()
class PROJECT25L_API ADSCharacter : public ADSCharacterBase
{
	GENERATED_BODY()
	
public:

	ADSCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetGimmickState(bool bShouldChange);
	void AddSkill(const int32 InputID);

	void SetSurroundingItem(AActor* Actor);
	AActor* GetSurroundingItem() const { return SurroundingItem.Get(); }

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_UseItem(int32 ItemID, int32 ItemCount);

	UDSInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	void SetJumpHeight(uint8 bIsRun);

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	TObjectPtr<UDSInventoryComponent> InventoryComponent;

	UPROPERTY()
	TWeakObjectPtr<AActor> SurroundingItem;

protected:
	/*Spring Arm, Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

protected:
	/*Character Setting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Jump")
	float NomalJumpHeight = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Jump")
	float RunJumpHeight = 40.f;

};
