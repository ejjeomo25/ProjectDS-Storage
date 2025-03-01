// Default
#include "Inventory/DSInventoryComponent.h"

// UE
#include "EngineUtils.h"

// Game
#include "Character/DSCharacter.h"
#include "GameData/DSGameDataSubsystem.h"
#include "GameData/Items/DSItemData.h"
#include "Item/DSItem.h"
#include "Item/DSItemAccessory.h"
#include "Item/DSItemGrenade.h"
#include "Item/DSItemPotion.h"
#include "Item/DSItemVehicle.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "System/DSEnums.h"
#include "DSLogChannels.h"

UDSInventoryComponent::UDSInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PersonalInventory()
{

}

void UDSInventoryComponent::UseItem(int32 ItemID)
{
	if (PersonalInventory.Contains(ItemID) == false)
	{
		return;
	}

	PersonalInventory[ItemID] -= 1; //한개 사용

	//이펙트 사용

	ServerRPC_UseItem(ItemID, PersonalInventory[ItemID]);
}

void UDSInventoryComponent::UseItem(int32 ItemID, int32 ItemCount)
{
	for (int i = 0; i < ItemCount; i++)
	{
		UseItem(ItemID);
	}
}

void UDSInventoryComponent::ServerRPC_UseItem_Implementation(int32 ItemID, int ClientItemCount)
{
	UWorld* World = GetWorld();

	check(World);

	//클라이언트에서는 이미 하나 사용함
	if ((PersonalInventory[ItemID] - 1) != ClientItemCount)
	{
		//이때도 Rollback 하고, 사용하지 않음.
		ClientRPC_RollbackItems(ItemID, PersonalInventory[ItemID]);
		return;
	}

	PersonalInventory[ItemID] -= 1; //한개 사용

	OnItemUsed(ItemID);

	APlayerController* LocalPlayertController = GetController<APlayerController>();

	//모든 클라이언트를 가지고와서 이펙트 사용!!
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && LocalPlayertController != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				ADSCharacter* OtherPlayer = Cast<ADSCharacter>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					//이펙트 실행
				}
			}
		}
	}

}

void UDSInventoryComponent::OnItemUsed(int32 ItemID)
{

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);

	check(DataSubsystem);

	EItemType ItemType = IDSItem::ConvertToItemType(ItemID);

	DS_LOG(DSItemLog, Warning, TEXT("ItemType %s"),*UEnum::GetValueAsString(ItemType));

	//ConvertToItemType에 의해
	FTableRowBase* ItemData = nullptr;

	IDSItem* SelectedItem = nullptr;

	//초기화 리스트를 사용해서 DataTable에 값을 초기화해준다.
	switch (ItemType)
	{
	case EItemType::Accessory:
		ItemData = DataSubsystem->GetItemData(EDataTableType::ItemAccessoryData, ItemID);
		SelectedItem = NewObject<UDSItemAccessory>();
		break;
	case EItemType::Grenade:
		ItemData = DataSubsystem->GetItemData(EDataTableType::ItemGrenadeData, ItemID);
		SelectedItem = NewObject<UDSItemGrenade>();
		break;
	case EItemType::Potion:
		ItemData = DataSubsystem->GetItemData(EDataTableType::ItemPotionData, ItemID);
		SelectedItem = NewObject<UDSItemPotion>();
		break;
	case EItemType::Vehicle:
		ItemData = DataSubsystem->GetItemData(EDataTableType::ItemVehicleData, ItemID);
		SelectedItem = NewObject<UDSItemVehicle>();
		break;
	}

	if (nullptr != SelectedItem)
	{
		//데이터 초기화
		SelectedItem->Initialize(ItemData);

		ADSPlayerState* PS = Cast<ADSPlayerState>(GetOwner());

		if (IsValid(PS))
		{
			ADSCharacterBase* Character = PS->GetPawn<ADSCharacterBase>();

			if (IsValid(Character))
			{
				//실제 Stat 적용 로직
				SelectedItem->UseItem(Character->GetStatComponent());
			}
		}
		
		DS_LOG(DSItemLog, Warning, TEXT("ItemID is valid"));
	}
	else
	{
		DS_LOG(DSItemLog, Warning, TEXT("ItemID is not valid"));
	}
}

void UDSInventoryComponent::StoreItems(int32 ItemID, int32 Count)
{
	if (PersonalInventory.Contains(ItemID) == false)
	{
		PersonalInventory.Add(ItemID, Count);
	}
	else
	{
		PersonalInventory[ItemID] += Count;
	}

	ServerRPC_StoreItems(ItemID, Count, PersonalInventory[ItemID]);
}

bool UDSInventoryComponent::ServerRPC_StoreItems_Validate(int32 ItemID, int Count, int ClientItemCount)
{
	if (PersonalInventory.Contains(ItemID))
	{
		if ((PersonalInventory[ItemID] + Count) != ClientItemCount)
		{
			ClientRPC_RollbackItems(ItemID, PersonalInventory[ItemID] + Count);
		}
	}

	return true;
}

void UDSInventoryComponent::ServerRPC_StoreItems_Implementation(int32 ItemID, int Count, int ClientItemCount)
{
	if (PersonalInventory.Contains(ItemID) == false)
	{
		PersonalInventory.Add(ItemID, Count);
	}
	else
	{
		PersonalInventory[ItemID] += Count;
	}
}

void UDSInventoryComponent::ClientRPC_RollbackItems_Implementation(int32 ItemID, int Count)
{
	if (PersonalInventory.Contains(ItemID))
	{
		PersonalInventory[ItemID] = Count;
	}
}
