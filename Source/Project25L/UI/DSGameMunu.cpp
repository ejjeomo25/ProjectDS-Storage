// Defualt
#include "UI/DSGameMunu.h"

// UE
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"

// Game
#include "DSLogChannels.h"
#include "UI/Inventory/DSInventoryWidget.h"
#include "Input/DSPlayerInputComponent.h"
#include "System/DSEventSystems.h"
#include "Character/DSCharacter.h"

void UDSGameMunu::ToggleInventory()
{
    ToggleWidget(InventoryWidget);
}

void UDSGameMunu::ToggleWidget(UUserWidget* Widget)
{
	if (!IsValid(WidgetSwitcher) || !IsValid(Widget))
	{
		return;
	}

	// 현재 활성화된 위젯 가져오기
	UWidget* ActiveWidget = WidgetSwitcher->GetActiveWidget();

	// 선택 로직: 현재 활성화된 위젯이 토글할 위젯인지 확인
	UWidget* NewWidget = (ActiveWidget == Widget) ? Cast<UWidget>(EmptyBorder) : Widget;

	// 위젯 변경
	WidgetSwitcher->SetActiveWidget(NewWidget);
}



void UDSGameMunu::NativeConstruct()
{
	DS_LOG(DSUILog, Log, TEXT("DSGameMunu"));
	Super::NativeConstruct();

	APlayerController* PC = GetOwningPlayer();
	if (IsValid(PC))
	{
		APawn* Pawn = PC->GetPawn();
		if (IsValid(Pawn))
		{
			ADSCharacter* Character = Cast<ADSCharacter>(Pawn);
			if (IsValid(Character))
			{
				DSEVENT_DELEGATE_BIND(Character->OnInventoryToggle, this, &UDSGameMunu::ToggleInventory);
			}
		}
	}
}
