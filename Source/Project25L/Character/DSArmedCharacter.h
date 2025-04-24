#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSArmedCharacter.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Unequipped UMETA(DisplayName = "Unequipped"),
	Equipped   UMETA(DisplayName = "Equipped"),
	Attack	   UMETA(DisplayName = "Attack")
};

UENUM(BlueprintType)
enum class EWeaponSocketType : uint8
{
	Stow UMETA(DisplayName = "Store weapons"),
	Equipped UMETA(DisplayName = "Equip Weapons")
};

class ADSWeapon;

UCLASS()
class PROJECT25L_API ADSArmedCharacter : public ADSCharacter
{
	GENERATED_BODY()
	
public:
	ADSArmedCharacter(const FObjectInitializer& ObjectInitializer);

public:
/*Getter 함수*/
	const ADSWeapon* GetWeapon() const { return Weapon; }
	ADSWeapon* GetWeapon() { return Weapon; }
	bool GetIsEquipped() { return bIsEquipped; }

	virtual float GetInputThreshold() override;

public:
/*무기를 장착하고, 무기와 관련된 애니메이션 몽타주를 실행하는 함수*/
	void Equip();
	
	void UnEquip();
	
	void MoveEquip();

	void LoadWeapon();

	void PlayAnimation(EWeaponState WeaponState);

protected:
/*RPC*/
	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon(EWeaponState EquipState);

	UFUNCTION(Client, Reliable)
	void ClientRPC_EquipWeapon(ADSArmedCharacter* Character, EWeaponState EquipState);
	
protected:
/*Unreal Engine 기본 함수*/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Transient)
	TMap<EWeaponState,TObjectPtr<UAnimMontage>> WeaponMontages;

	UPROPERTY(Transient)
	uint8 bIsEquipped : 1;

	UPROPERTY(Transient, Replicated)
	TObjectPtr<ADSWeapon> Weapon;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	TMap<EWeaponSocketType, FName> SocketName;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Weapon")
	EWeaponType WeaponType;

};
