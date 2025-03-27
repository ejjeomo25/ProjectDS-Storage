// Default
#include "Item/DSItemPotion.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemStatData.h"

UDSItemPotion::UDSItemPotion()
	: Super()
	, Duration(0)
{
}

void UDSItemPotion::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemStatData* ItemData = static_cast<FDSItemStatData*>(DataTableRow);
	if (nullptr != ItemData)
	{
	}
}

void UDSItemPotion::UseItem(const UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Potion"));
}
