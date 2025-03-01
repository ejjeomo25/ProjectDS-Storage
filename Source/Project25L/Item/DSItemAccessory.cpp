
// Default
#include "Item/DSItemAccessory.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemAccessoryData.h"

UDSItemAccessory::UDSItemAccessory()
	: Super()
	, Duration(0)
{
}

void UDSItemAccessory::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemAccessoryData* ItemData = static_cast<FDSItemAccessoryData*>(DataTableRow);

	if (nullptr != ItemData)
	{
		Duration = ItemData->Duration;
	}
}

void UDSItemAccessory::UseItem(const UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Accessory"));
}
