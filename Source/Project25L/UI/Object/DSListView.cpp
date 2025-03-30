//Default
#include "UI/Object/DSListView.h"

// Game
#include "System/DSUIManagerSubsystem.h"
#include "Character/DSCharacter.h"
#include "DSLogChannels.h"
#include "UI/DSItemEntry.h"

void UDSListView::HandleListEntryHovered(UUserWidget& EntryWidget)
{
	Super::HandleListEntryHovered(EntryWidget);

	// 현재 포커스되어 있는 Widget을 가져온다.
	UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(&EntryWidget);

	if (IsValid(ItemEntry))
	{
		// 현재 EntryWidget을 킨다.
		ItemEntry->SetFocus(true);
	}
}

void UDSListView::HandleListEntryUnhovered(UUserWidget& EntryWidget)
{
	Super::HandleListEntryUnhovered(EntryWidget);
	//이전 위젯 포커스를 끊어낸다.
	UDSItemEntry* ItemEntry = Cast<UDSItemEntry>(&EntryWidget);

	if (IsValid(ItemEntry))
	{
		// 현재 EntryWidget을 끈다.
		ItemEntry->SetFocus(false);
	}

}

void UDSListView::OnItemClickedInternal(UObject* Item)
{
	Super::OnItemClickedInternal(Item);
	
	int32 SelectedIdx = GetIndexForItem(Item);

	APlayerController* PlayerController = GetOwningPlayer();

	if (IsValid(PlayerController) == false)
	{
		return;
	}

	ADSCharacter* Character = PlayerController->GetPawn<ADSCharacter>();

	if (IsValid(Character))
	{
		Character->TryPickupItem(SelectedIdx);
	}
}
