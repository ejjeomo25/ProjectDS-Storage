// Default
#include "Item/DSItemGrenade.h"

// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemGrenadeData.h"

UDSItemGrenade::UDSItemGrenade()
	: Super()
	, ThrowRange(0)
	, ThrowDamage(0)
{
}

void UDSItemGrenade::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemGrenadeData* ItemData = static_cast<FDSItemGrenadeData*>(DataTableRow);

	if (nullptr != ItemData)
	{
		ThrowDamage = ItemData->ThrowDamage;
		ThrowRange = ItemData->ThrowRange;
	}
}

void UDSItemGrenade::UseItem(const UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item Grenade"));
}
