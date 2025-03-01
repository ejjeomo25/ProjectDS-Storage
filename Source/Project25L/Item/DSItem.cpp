// Default
#include "Item/DSItem.h"

EItemType IDSItem::ConvertToItemType(int32 ItemID)
{
	EItemType ItemType = EItemType::None;

	int Dist = (static_cast<int32>(EItemType::Vehicle) - static_cast<int32>(EItemType::None));

	for (int32 ItemTypeIdx = static_cast<int32>(EItemType::None); ItemTypeIdx <= static_cast<int32>(EItemType::Max); ItemTypeIdx += Dist)
	{
		int DivIdx = ItemTypeIdx / ItemID;

		// 1로 딱 떨어지는 구간을 확인한다.
		if (DivIdx >= 1)
		{
			ItemType = static_cast<EItemType>(ItemTypeIdx - Dist);
			break;
		}
	}
	
	return ItemType;
}
