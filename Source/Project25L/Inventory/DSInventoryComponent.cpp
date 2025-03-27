// Default
#include "Inventory/DSInventoryComponent.h"

// UE
#include "EngineUtils.h"

// Game
#include "Character/DSCharacter.h"
#include "DSLogChannels.h"
#include "GameData/DSGameDataSubsystem.h"
#include "GameData/Items/DSItemData.h"
#include "Item/DSItem.h"
#include "Item/DSItemAccessory.h"
#include "Item/DSItemActor.h"
#include "Item/DSItemGrenade.h"
#include "Item/DSItemPotion.h"
#include "Item/DSItemVehicle.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "System/DSEnums.h"
#include "System/DSEventSystems.h"

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

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		//서버의 경우 위에서 사용했기 때문에 제거하지 않는다. 안그러면 이중으로 제거되는 문제가 있다.
		PersonalInventory[ItemID] -= 1; //한개 사용
	}
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
	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		if (PersonalInventory[ItemID] - 1 != ClientItemCount)
		{
			//이때도 Rollback 하고, 사용하지 않음.
			ClientRPC_RollbackItems(ItemID, PersonalInventory[ItemID]);
			return;
		}
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

	//ConvertToItemType에 의해
	FTableRowBase* ItemData = nullptr;

	IDSItem* SelectedItem = nullptr;

	//초기화 리스트를 사용해서 DataTable에 값을 초기화해준다.
	switch (ItemType)
	{
	case EItemType::Accessory:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemAccessoryData, ItemID);
		SelectedItem = NewObject<UDSItemAccessory>();
		break;
	case EItemType::Grenade:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemGrenadeData, ItemID);
		SelectedItem = NewObject<UDSItemGrenade>();
		break;
	case EItemType::Potion:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemPotionData, ItemID);
		SelectedItem = NewObject<UDSItemPotion>();
		break;
	case EItemType::Vehicle:
		ItemData = DataSubsystem->GetDataRowByID(EDataTableType::ItemVehicleData, ItemID);
		SelectedItem = NewObject<UDSItemVehicle>();
		break;
	}

	if (nullptr != SelectedItem)
	{
		//데이터 초기화
		SelectedItem->Initialize(ItemData);

		ADSCharacterBase* Character = Cast<ADSCharacterBase>(GetOwner());

		if (IsValid(Character))
		{
				//실제 Stat 적용 로직
			SelectedItem->UseItem(Character->GetStatComponent());
		}
		
		DS_LOG(DSItemLog, Warning, TEXT("ItemID is valid"));
	}
	else
	{
		DS_LOG(DSItemLog, Warning, TEXT("ItemID is not valid"));
	}
}

void UDSInventoryComponent::PrintItem()
{
	for (const auto& Item : PersonalInventory)
	{
		if (IsValid(GEngine))
		{
			FString Msg = FString::Printf(TEXT("ItemID %d Cnt = %d"),Item.Key, Item.Value);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);
		}
	}
}

void UDSInventoryComponent::StoreItems(ADSItemActor* ItemActor,int32 ID, int32 Count)
{

	int32 ItemID = ID;

	if (IsValid(ItemActor))
	{
		ItemID = ItemActor->GetID();
	}

	if (!PersonalInventory.Contains(ItemID))
	{
		PersonalInventory.Add(ItemID, Count);
	}
	else
	{
		PersonalInventory[ItemID] += Count;
	}

	DS_LOG(DSItemLog, Log, TEXT("Client ItemID %d ItemCount %d"), ItemID, PersonalInventory[ItemID]);

	// 🔹 리슨 서버에서는 별도의 RPC 호출 없이 로컬에서 처리
	if (GetOwner()->HasAuthority())
	{
		if (IsValid(ItemActor))
		{
			ItemActor->SetLifeSpan(0.3f);
		}
		return;
	}

	ServerRPC_StoreItems(ItemActor, ID, Count, PersonalInventory[ItemID]);
}

bool UDSInventoryComponent::ServerRPC_StoreItems_Validate(ADSItemActor* ItemActor, int32 ID, int32 Count, int32 ClientItemCount)
{

	int32 ItemID = ID;

	if (IsValid(ItemActor))
	{
		ItemID = ItemActor->GetID();
	}

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		//클라이언트만 검사한다. 서버는 항상 참이다.
		if (PersonalInventory.Contains(ItemID))
		{
			if ((PersonalInventory[ItemID] + Count) != ClientItemCount)
			{
				ClientRPC_RollbackItems(ItemID, PersonalInventory[ItemID] + Count);
			}
		}
	}

	return true;
}

void UDSInventoryComponent::ServerRPC_StoreItems_Implementation(ADSItemActor* ItemActor, int32 ID, int32 Count, int32 ClientItemCount)
{

	int32 ItemID = ID;

	if (IsValid(ItemActor))
	{
		ItemID = ItemActor->GetID();
	}


	// 전용 서버에서도 아이템을 저장하도록 수정
	if (!PersonalInventory.Contains(ItemID))
	{
		PersonalInventory.Add(ItemID, Count);
	}
	else
	{
		PersonalInventory[ItemID] += Count;
	}

	// 클라이언트와 서버의 아이템 개수가 맞지 않을 경우 로그 출력
	if (ClientItemCount != PersonalInventory[ItemID])
	{
		DS_LOG(DSItemLog, Warning, TEXT("[Desync] ClientItemCount(%d) != ServerItemCount(%d) for ItemID(%d)"),
			ClientItemCount, PersonalInventory[ItemID], ItemID);
	}

	if (IsValid(ItemActor))
	{
		ItemActor->SetLifeSpan(0.3f);
	}
}

void UDSInventoryComponent::ClientRPC_RollbackItems_Implementation(int32 ItemID, int32 Count)
{
	if (PersonalInventory.Contains(ItemID))
	{
		PersonalInventory[ItemID] = Count;
	}
}
