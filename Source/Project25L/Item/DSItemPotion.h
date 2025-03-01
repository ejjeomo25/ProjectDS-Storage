#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "UObject/NoExportTypes.h"

// Game
#include "Item/DSItem.h"

// UHT
#include "DSItemPotion.generated.h"

UCLASS()
class PROJECT25L_API UDSItemPotion : public UObject, public IDSItem
{
	GENERATED_BODY()
	
public:
	UDSItemPotion();

	virtual void Initialize(FTableRowBase* DataTableRow) override;
	virtual void UseItem(const UDSStatComponent* Stat) override;

protected:
	
	UPROPERTY(Transient)
	float Duration;
};
