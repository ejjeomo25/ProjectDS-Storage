#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/Interface.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSItem.generated.h"

class UDSStatComponent;

UINTERFACE(MinimalAPI)
class UDSItem : public UInterface
{
	GENERATED_BODY()
};

class PROJECT25L_API IDSItem
{
	GENERATED_BODY()

public:

	virtual void UseItem(const UDSStatComponent* Stat) = 0;
	virtual void Initialize(FTableRowBase* DataTableRow) = 0;
	static EItemType ConvertToItemType(int32 ItemType);
};
