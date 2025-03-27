#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItemData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FDSItemData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemData()
		: ItemName(TEXT(""))
		, ItemHolder(0x00)
		, Probability(0.0f)
		, ActorClass(nullptr)
		, Description(TEXT(""))
	{}

	UPROPERTY(EditAnywhere, Category = Name)
	FString ItemName;
	
	UPROPERTY(EditAnywhere, Category = Holder)
	uint8 ItemHolder;

	UPROPERTY(EditAnywhere, Category = Name)
	TSoftObjectPtr<UTexture2D> ItemLogo;

	UPROPERTY(EditAnywhere, Category = Prop)
	float Probability;

	UPROPERTY(EditAnywhere, Category = Actor)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = Desc)
	FString Description;
	
};

USTRUCT(BlueprintType)
struct FDSItemInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemInfo() {}

	FDSItemInfo(int32 ID) : ID(ID) {}

	UPROPERTY()
	int32 ID;

	// 구조체의 변수들을 리플리케이션하기 위해 GetLifetimeReplicatedProps() 필요
	bool operator==(const FDSItemInfo& Other) const
	{
		return ID == Other.ID;
	}

	// NetSerialize 구현
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << ID;
		bOutSuccess = true;
		return true;
	}
};

UCLASS()
class PROJECT25L_API UDSItemUI : public UObject
{
	GENERATED_BODY()
	
public:

	int32 ID;
	bool bIsSelected;
};