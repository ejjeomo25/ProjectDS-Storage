#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class PROJECT25L_API ADSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSWeapon();

protected:

	virtual void BeginPlay() override;
	virtual void InitializeData();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(Transient, Replicated)
	float InputThreshold;

	UPROPERTY(EditAnywhere, Category = "Setting | Type")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Setting | Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh;

};
