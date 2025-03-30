#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/DSCharacter.h"
#include "System/DSEnums.h"

// UHT
#include "DSArmedCharacter.generated.h"

UENUM(BlueprintType)
enum class EWeaponEquipState : uint8
{
	Unequipped UMETA(DisplayName = "Unequipped"),
	Equipped   UMETA(DisplayName = "Equipped")
};

class ADSWeapon;

UCLASS()
class PROJECT25L_API ADSArmedCharacter : public ADSCharacter
{
	GENERATED_BODY()
	
public:
	ADSArmedCharacter(const FObjectInitializer& ObjectInitializer);

	bool GetIsEquipped() { return bIsEquipped; }
	void Equip();
	void UnEquip();
	void MoveEquip();
	virtual float GetInputThreshold() override;

	const ADSWeapon* GetWeapon() const { return Weapon; }
protected:

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon(EWeaponEquipState EquipState);

	UFUNCTION(Client, Reliable)
	void ClientRPC_EquipWeapon(ADSArmedCharacter* Character, EWeaponEquipState EquipState);

	void LoadWeapon();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Transient)
	TMap<EWeaponEquipState,TObjectPtr<UAnimMontage>> EquipMontages;

	UPROPERTY(Transient)
	bool bIsEquipped;

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ADSWeapon> Weapon;

	UPROPERTY(EditAnywhere, Category = Weapon)
	EWeaponType WeaponType;

};
