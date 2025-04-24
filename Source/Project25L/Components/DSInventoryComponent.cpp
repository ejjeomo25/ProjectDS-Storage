// Default
#include "Components/DSInventoryComponent.h"

// UE
#include "EngineUtils.h"

// Game
#include "Character/Characters/DSCharacter.h"
#include "DSLogChannels.h"
#include "System/DSGameDataSubsystem.h"
#include "GameData/Items/DSItemData.h"
#include "Item/DSItem.h"
#include "Item/DSItemAccessory.h"
#include "Item/DSItemActor.h"
#include "Item/DSItemGrenade.h"
#include "Item/DSItemPotion.h"
#include "Item/DSItemVehicle.h"
#include "Player/DSPlayerController.h"
#include "Player/DSPlayerState.h"
#include "GameData/DSEnums.h"
#include "System/DSEventSystems.h"

UDSInventoryComponent::UDSInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PersonalInventory()
{

}

void UDSInventoryComponent::UseItem(int32 ItemIdx)
{
	if (false == PersonalInventory.IsValidIndex(ItemIdx))
	{
		return;
	}

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		//서버의 경우 위에서 사용했기 때문에 제거하지 않는다. 안그러면 이중으로 제거되는 문제가 있다.
		PersonalInventory.RemoveAt(ItemIdx); //쒓컻 ъ슜
	}
	//이펙트 사용
	ServerRPC_UseItem(ItemIdx, PersonalInventory.Num());
}

bool UDSInventoryComponent::ServerRPC_UseItem_Validate(int32 ItemIdx, int ClientItemCount)
{

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		//클라이언트만 검사한다. 서버는 항상 참이다.
		if (PersonalInventory.Num() - 1 != ClientItemCount)
		{
			//뒤에 들어간 아이템을 제거하면 된다.
			//이때는 Idx Add 해주어야 한다.
			ClientRPC_RollbackItems(true, PersonalInventory.Top());
		}
	}
	return true;
}
void UDSInventoryComponent::ServerRPC_UseItem_Implementation(int32 ItemIdx, int ClientItemCount)
{
	UWorld* World = GetWorld();

	check(World);

	int UsedItemID = PersonalInventory.Top().ID;

	PersonalInventory.RemoveAt(ItemIdx); //한개 사용
	
	OnItemUsed(UsedItemID);

	APlayerController* LocalPlayertController = GetController<APlayerController>();

	//모든 클라이언트를 가지고와서 이펙트 사용!!
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (IsValid(PlayerController) && LocalPlayertController != PlayerController)
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

void UDSInventoryComponent::OnRegister()
{
	Super::OnRegister();

	const int32 TotalSlots = PersonalInventoryInfo.Columns * PersonalInventoryInfo.Rows;
	PersonalInventory.SetNum(TotalSlots);
	for (FDSItemInfo& Slot : PersonalInventory)
	{
		Slot.ID = 0;
		Slot.isRotated = false;
	}
}

void UDSInventoryComponent::PrintItem()
{
	for (const auto& Item : PersonalInventory)
	{
		if (IsValid(GEngine))
		{
			FString Msg = FString::Printf(TEXT("ItemID %d"),Item.ID);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);
		}
	}
}

bool UDSInventoryComponent::IsRoomAvailable(const FDSItemInfo& ItemInfo, int32 TopLeftIndex)
{
	return GetEmptyTileIndex(ItemInfo, TopLeftIndex);
}

FIntPoint UDSInventoryComponent::IndextoTile(int32 Index, int32 Columns)
{
	FIntPoint Tile;

	Tile.X = Index % Columns;
	Tile.Y = Index / Columns;

	return Tile;

}

int32 UDSInventoryComponent::TiletoIndex(FIntPoint DSTile, int32 Columns)
{
	return DSTile.X + DSTile.Y * Columns;
}

bool UDSInventoryComponent::GetEmptyTileIndex(const FDSItemInfo& ItemInfo, int32 TopLeftIndex)
{
	FIntPoint StartTile = IndextoTile(TopLeftIndex, PersonalInventoryInfo.Columns);

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);
	
	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));
	if (nullptr == ItemData)
	{
		return false;
	}
	
	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;
	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint TileToCheck(StartTile.X + x, StartTile.Y + y);

			
			if (TileToCheck.X < 0 || TileToCheck.X >= PersonalInventoryInfo.Columns ||
				TileToCheck.Y < 0 || TileToCheck.Y >= PersonalInventoryInfo.Rows)
			{
				return false;
			}

			int32 Index = TiletoIndex(TileToCheck, PersonalInventoryInfo.Columns);
			if (IsItemAtIndex(Index))
			{
				return false;
			}
		}
	}

	return true;

}

bool UDSInventoryComponent::IsItemAtIndex(int32 Index)
{
	if (PersonalInventory.IsValidIndex(Index))
	{
		return PersonalInventory[Index].ID > 0;
	}
	return false;

}

void UDSInventoryComponent::AddItemAt(const FDSItemInfo& ItemInfo, int32 TopLeftIndex)
{

	FIntPoint StartTile = IndextoTile(TopLeftIndex, PersonalInventoryInfo.Columns);

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;
	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint Tile;
			Tile.X = StartTile.X + x;
			Tile.Y = StartTile.Y + y;

			int32 Index = TiletoIndex(Tile, PersonalInventoryInfo.Columns);

			FDSItemInfo& Slot = PersonalInventory[Index];
			Slot.ID = ItemInfo.ID;
			Slot.isRotated = ItemInfo.isRotated;

		}
	}

	RegisterItemToAllItems();
	DSEVENT_DELEGATE_INVOKE(OnInventoryChanged);

}

void UDSInventoryComponent::RegisterItemToAllItems()
{
	TSet<int32> OccupiedIndices;
	AllItems.Empty();

	int32 Columns = PersonalInventoryInfo.Columns;
	int32 Rows = PersonalInventoryInfo.Rows;

	for (int32 Index = 0; Index < Columns * Rows; ++Index)
	{
		if (OccupiedIndices.Contains(Index))
		{
			continue; 
		}

		if (!PersonalInventory.IsValidIndex(Index)) continue;

		const FDSItemInfo& Slot = PersonalInventory[Index];

		if (Slot.ID == 0)
		{
			continue;
		}

		UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

		check(DataManager);

		FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, Slot.ID));

		if (nullptr == ItemData)
		{
			continue;
		}

		FIntPoint Dimensions = ItemData->Dimensions;
		const bool bIsRotated = Slot.isRotated;
		const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;
		FIntPoint TopLeftTile = IndextoTile(Index, Columns);
		AllItems.Add(TopLeftTile, Slot); 

		for (int32 y= 0; y < FinalSize.Y; ++y)
		{
			for (int32 x = 0; x < FinalSize.X; ++x)
			{
				FIntPoint Tile;
				Tile.X = TopLeftTile.X + x;
				Tile.Y = TopLeftTile.Y + y;

				int32 OccupiedIndex = TiletoIndex(Tile, Columns);

				if (PersonalInventory.IsValidIndex(OccupiedIndex))
				{
					OccupiedIndices.Add(OccupiedIndex);
				}
			}
		}
	}
}

void UDSInventoryComponent::RemoveItem(const FDSItemInfo& ItemInfo, FIntPoint TopLeftIndex)
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);
	check(DataManager);

	FDSItemData* ItemData = static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemInfo.ID));

	if (nullptr == ItemData)
	{
		return;
	}

	const FIntPoint Dimensions = ItemData->Dimensions;
	const bool bIsRotated = ItemInfo.isRotated;
	const FIntPoint FinalSize = bIsRotated ? FIntPoint(Dimensions.Y, Dimensions.X) : Dimensions;

	const int32 Columns = PersonalInventoryInfo.Columns;

	for (int32 y = 0; y < FinalSize.Y; ++y)
	{
		for (int32 x = 0; x < FinalSize.X; ++x)
		{
			FIntPoint Tile = FIntPoint(TopLeftIndex.X + x, TopLeftIndex.Y + y);
			int32 Index = TiletoIndex(Tile, Columns);

			if (PersonalInventory.IsValidIndex(Index))
			{
				FDSItemInfo& Slot = PersonalInventory[Index];
				Slot.ID = 0;
				Slot.isRotated = false;
			}
		}
	}
	RegisterItemToAllItems();
	DSEVENT_DELEGATE_INVOKE(OnInventoryChanged);
}

bool UDSInventoryComponent::StoreItems(ADSItemActor* ItemActor,const FDSItemInfo &ItemData)
{
	bool bIsItemStored = false;
	for (int32 Index = 0; Index < PersonalInventoryInfo.Columns * PersonalInventoryInfo.Rows; ++Index)
	{
		bIsItemStored = IsRoomAvailable(ItemData, Index);
		if(bIsItemStored)
		{
			AddItemAt(ItemData, Index);
			DS_LOG(DSItemLog, Warning, TEXT("StoreItems"));
			break;
		}
	}

	// 아이템이 저장됨 
	if (bIsItemStored)
	{
		// 🔹 리슨 서버에서는 별도의 RPC 호출 없이 로컬에서 처리
		if (GetOwner()->HasAuthority())
		{
			if (IsValid(ItemActor))
			{
				ItemActor->SetLifeSpan(0.3f);
			}
			return bIsItemStored;
		}

		ServerRPC_StoreItems(ItemActor, ItemData, PersonalInventory.Num());
	}
	return bIsItemStored;
}

bool UDSInventoryComponent::ServerRPC_StoreItems_Validate(ADSItemActor* ItemActor, const FDSItemInfo& ItemData, int32 ClientItemCount)
{

	if (GetNetMode() != ENetMode::NM_ListenServer)
	{
		//클라이언트만 검사한다. 서버는 항상 참이다.
		if (PersonalInventory.Num() != ClientItemCount - 1)
		{
			//뒤에 들어간 아이템을 제거하면 된다.
			ClientRPC_RollbackItems(false, FDSItemInfo());
		}
	}

	return true;
}

void UDSInventoryComponent::ServerRPC_StoreItems_Implementation(ADSItemActor* ItemActor, const FDSItemInfo& ItemData, int32 ClientItemCount)
{

	// 전용 서버에서도 아이템을 저장하도록 수정
	PersonalInventory.Add(ItemData);

	if (IsValid(ItemActor))
	{
		ItemActor->SetLifeSpan(0.3f);
	}
}

void UDSInventoryComponent::ClientRPC_RollbackItems_Implementation(bool bIsPlus, FDSItemInfo ItemData)
{
	if (false == PersonalInventory.IsEmpty())
	{
		if (false == bIsPlus)
		{
			PersonalInventory.Pop();  //뒤에 있는 개수를 뺀다.
		}
		else
		{
			PersonalInventory.Add(ItemData);
		}
	}
}
