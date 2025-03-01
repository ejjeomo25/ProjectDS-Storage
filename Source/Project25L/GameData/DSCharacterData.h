#pragma once

#include "CoreMinimal.h"
#include "System/DSEnums.h"
#include "DSCharacterData.generated.h"

UCLASS()
class PROJECT25L_API UDSCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("DSCharacterData", GetFName());
	}


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
