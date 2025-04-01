// Default
#include "UI/Item/DSItemEntry.h"

// UE
#include "Components/TextBlock.h"
#include "Components/Image.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "Item/DSItemActor.h"
#include "GameData/Items/DSItemData.h"

void UDSItemEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UDSItemUI* ItemActor = Cast<UDSItemUI>(ListItemObject);

	if (false == IsValid(ItemActor))
	{
		return;
	}

	if (false == IsValid(Text_ItemName))
	{
		return;
	}

	if (false == IsValid(IMG_Background))
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

	if (false == IsValid(IMG_Background))
	{
		return;
	}

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
	UDSGameDataSubsystem::StreamableManager.LoadSynchronous(Data);

	if (Data.IsValid())
	{
		if (IsValid(IMG_ItemIcon))
		{
			IMG_ItemIcon->SetBrushFromTexture(Data.Get());
		}
	}
}

