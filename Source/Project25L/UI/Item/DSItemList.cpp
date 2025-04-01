// Default
#include "UI/Item/DSItemList.h"

// UE

// Game
#include "UI/Object/DSListView.h"
#include "Character/Characters/DSCharacter.h"
#include "GameData/Items/DSItemData.h"
#include "System/DSEventSystems.h"
#include "System/DSUIManagerSubsystem.h"
#include "UI/Item/DSItemEntry.h"

#include "DSLogChannels.h"

UDSItemList::UDSItemList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SelectedIdx(0)
{
}

void UDSItemList::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDSItemList::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(ListView_Items))
	{
		ListView_Items->ClearListItems();
	}
}

void UDSItemList::AddItems(TArray<FDSItemInfo>& ItemInfos)
{
	if (IsValid(ListView_Items))
	{
		//이미 그렸음.
		//이 부분 수정해야할 듯 . 예외의 경우를 만들지 말고,, 실제 동작할 때 델리게이트 연결을 적절히 바꿔야한다고 생각함
		//예를 들어서.. 연결 부분이 
		if (ListView_Items->GetNumItems() != 0)
		{
			return;
		}

		ListView_Items->ClearListItems();

		for (int DrawIdx = 0; DrawIdx < ItemInfos.Num(); DrawIdx++)
		{
			UDSItemUI* ItemUI = NewObject<UDSItemUI>();

			if (IsValid(ItemUI))
			{
				ItemUI->ID = ItemInfos[DrawIdx].ID;
				ListView_Items->AddItem(ItemUI);
			}
		}
		//처음 아이템을 리셋할 때 AddItems가 호출되어지기 때문에 SelectedIdx=0으로 고정되어 있어야 한다.
		SelectedIdx = 0;

		UDSItemUI* ItemUI = Cast<UDSItemUI>(ListView_Items->GetItemAt(SelectedIdx));

		if (IsValid(ItemUI))
		{
			ItemUI->bIsSelected = true;
		}

		ListView_Items->SetSelectedIndex(SelectedIdx);
	}

}

void UDSItemList::RemoveItem(int32 IndexToRemove)
{
	if (IsValid(ListView_Items))
	{
		UObject* RemoveItem = ListView_Items->GetItemAt(IndexToRemove);;

		if (IsValid(RemoveItem))
		{
			ListView_Items->RemoveItem(RemoveItem);
		}
		int32 MaxNum = ListView_Items->GetNumItems();
		int32 PreSelected = SelectedIdx;

		if (MaxNum > 0)
		{
			SelectedIdx = (SelectedIdx + 1) % MaxNum;
			FocusOnItemEntry(PreSelected);
		}
	}
}

void UDSItemList::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (false == IsValid(ListView_Items))
	{
		DS_LOG(DSNetLog, Warning, TEXT("ListView_Items is empty"));
		return;
	}
	const int32 ItemNum = ListView_Items->GetNumItems();

	if (ItemNum <= 0)
	{
		return;
	}

	const float WheelDelta = InMouseEvent.GetWheelDelta();
	int32 PreSelectedIdx = SelectedIdx;

	if (WheelDelta >= 1.0f)
	{
		//위로 올릴 경우,인덱스 감소
		SelectedIdx = (ItemNum + (SelectedIdx - 1)) % ItemNum;
	}
	else if (WheelDelta < 0.0f)
	{
		//아래로 내릴 경우, 인덱스 증가
		SelectedIdx = (SelectedIdx + 1) % ItemNum;
	}
	if (PreSelectedIdx != SelectedIdx)
	{
		FocusOnItemEntry(PreSelectedIdx);
	}
}

FReply UDSItemList::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetCharacter() == 'F')
	{
		ADSCharacter* Character = GetOwningPlayerPawn<ADSCharacter>();

		if (IsValid(Character))
		{
			Character->TryPickupItem(SelectedIdx);
		}
	}
	
	return FReply::Handled();
}
void UDSItemList::FocusOnItemEntry(int PreSelectedIdx)
{
	UDSItemUI* PreItemUI = Cast<UDSItemUI>(ListView_Items->GetItemAt(PreSelectedIdx));

	if (IsValid(PreItemUI))
	{
		UDSItemEntry* PreItemEntry = Cast<UDSItemEntry>(ListView_Items->GetEntryWidgetFromItem(PreItemUI));

		if (IsValid(PreItemEntry))
		{
			PreItemEntry->SetFocus(false);
		}
	}

	UDSItemUI* ItemUI = Cast<UDSItemUI>(ListView_Items->GetItemAt(SelectedIdx));

	if (IsValid(ItemUI))
	{
		UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(ListView_Items->GetEntryWidgetFromItem(ItemUI));

		if (IsValid(ItemEntry))
		{
			ItemEntry->SetFocus(true);
		}
	}

	ListView_Items->SetSelectedIndex(SelectedIdx);
	ListView_Items->RequestRefresh();

}
