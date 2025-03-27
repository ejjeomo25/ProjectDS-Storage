// Default
#include "UI/DSItemEntry.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

// Game
#include "GameData/DSGameDataSubsystem.h"
#include "Item/DSItemActor.h"
#include "GameData/Items/DSItemData.h"

void UDSItemEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UDSItemUI* ItemActor = Cast<UDSItemUI>(ListItemObject);

	if (IsValid(ItemActor) == false)
	{
		return;
	}

	UDSGameDataSubsystem *DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	int32 ItemID = ItemActor->ID;

	FDSItemData* ItemData= static_cast<FDSItemData*>(DataManager->GetDataRowByID(EDataTableType::ItemData, ItemID));

	if (nullptr != ItemData)
	{
		Text_ItemName->SetText(FText::FromString(ItemData->ItemName));
		LoadImg(ItemData->ItemLogo);
	}

	if (ItemActor->bIsSelected)
	{
		IMG_Background->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		IMG_Background->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UDSItemEntry::SetFocus(bool bIsFocus)
{
	if (bIsFocus)
	{
		IMG_Background->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		IMG_Background->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDSItemEntry::LoadImg(TSoftObjectPtr<UTexture2D>& Data)
{
	FStreamableManager StreamableManager;

	StreamableManager.LoadSynchronous(Data);

	if (Data.IsValid())
	{
		IMG_ItemIcon->SetBrushFromTexture(Data.Get());
	}
}

