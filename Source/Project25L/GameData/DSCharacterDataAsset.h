#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "GameData/DSPrimaryDataAsset.h"
#include "System/DSEnums.h"

// Game

// UHT
#include "DSCharacterDataAsset.generated.h"


UCLASS()
class PROJECT25L_API UDSCharacterDataAsset : public UDSPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual uint8 GetKey() override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	ECharacterType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TSoftClassPtr<UAnimInstance> AnimBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TSoftObjectPtr<UPhysicsAsset> PhysicsAsset;
};
