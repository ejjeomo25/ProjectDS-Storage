// Default
#include "Item/DSItemVehicle.h"
// Game
#include "DSLogChannels.h"
#include "GameData/Items/DSItemVehicleData.h"

UDSItemVehicle::UDSItemVehicle()
	: Super()
	, Duration(0)
	, WalkSpeed(0)
	, RunSpeed(0)
	, JumpHeight(0)
	, bCanFly(false)
{

}

void UDSItemVehicle::UseItem(const UDSStatComponent* Stat)
{
	// 아이템 사용에 대한 정의 합니다.
	DS_LOG(DSItemLog, Log, TEXT("Use Item UDSItemVehicle"));
}

void UDSItemVehicle::Initialize(FTableRowBase* DataTableRow)
{
	FDSItemVehicleData* ItemData = static_cast<FDSItemVehicleData*>(DataTableRow);

	if (nullptr != ItemData)
	{
		Duration = ItemData->Duration;
		WalkSpeed = ItemData->WalkSpeed;
		RunSpeed = ItemData->RunSpeed;
		JumpHeight = ItemData->JumpHeight;
		bCanFly = ItemData->bCanFly;
	}
}
