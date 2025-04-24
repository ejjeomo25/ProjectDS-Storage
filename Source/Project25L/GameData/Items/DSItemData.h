#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSItemData.generated.h"

class UTexture2D;
class UMaterialInterface;

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

	UPROPERTY(EditAnywhere, Category = Name)
	TSoftObjectPtr<UMaterialInterface> ItemTileMaterial;

	UPROPERTY(EditAnywhere, Category = Name)
	TSoftObjectPtr<UMaterialInterface> ItemTileMaterialRotated;

	UPROPERTY(EditAnywhere, Category = Prop)
	float Probability;

	UPROPERTY(EditAnywhere, Category = Actor)
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = Desc)
	FString Description;

	UPROPERTY(EditAnywhere, Category = Dimensions)
	FIntPoint Dimensions;
	
};

USTRUCT(BlueprintType)
struct FDSItemInfo
{
	GENERATED_USTRUCT_BODY()

public:
	FDSItemInfo() {}

	FDSItemInfo(int32 ID) : ID(ID), isRotated(false) {}

	UPROPERTY()
	int32 ID;

	UPROPERTY()
	uint8 isRotated : 1;

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
	uint8 bIsSelected : 1;
};



USTRUCT(BlueprintType)
struct FPersonalInventoryInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TileSize;
};


USTRUCT(BlueprintType)
struct FDSLine
{	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D End;

	FDSLine()
		: Start(FVector2D::ZeroVector), End(FVector2D::ZeroVector)
	{
	}

	FDSLine(float X1, float Y1, float X2, float Y2)
		: Start(FVector2D(X1, Y1)), End(FVector2D(X2, Y2))
	{
	}

};


// STRUCT(BlueprintType)
// truct FIntPoint
// 	
// 	GENERATED_BODY()
// 	UPROPERTY()
// 	int X;
// 
// 	UPROPERTY()
// 	int Y;
// 
// 	bool operator==(const FIntPoint& Other) const
// 	{
// 		return X == Other.X && Y == Other.Y;
// 	}
// ;