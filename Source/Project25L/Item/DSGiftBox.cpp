// Default
#include "Item/DSGiftBox.h"

// UE
#include "Components/SphereComponent.h"

// Game
#include "Character/DSCharacter.h"
#include "System/DSEventSystems.h"
#include "system/DSUIManagerSubsystem.h"
#include "UI/DSWidgetLayer.h"
#include "UI/DSItemList.h"

ADSGiftBox::ADSGiftBox()
	: Super()
	, ItemListWidget(nullptr)
{
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSGiftBox::OnComponentBeginOverlap_Child);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSGiftBox::OnComponentEndOverlap_Child);
}

void ADSGiftBox::ServerRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	MulticastRPC_RemoveItemData(IndexToRemove);
}

void ADSGiftBox::MulticastRPC_RemoveItemData_Implementation(int32 IndexToRemove)
{
	if (StoredItems.Num() > IndexToRemove)
	{
		StoredItems.RemoveAt(IndexToRemove); //제거한다. 

		//그리고 UI를 업데이트 할 수 있도록 델리게이트나 Widget 업데이트 요청을 한다 => 각 로컬에서만 동작하도록 한다.
		DSEVENT_DELEGATE_INVOKE(OnRemoveItemWidget, IndexToRemove);

		if (HasAuthority())
		{
			if (StoredItems.Num() <= 0)
			{
				SetLifeSpan(0.5f);
			}
		}
	}
}


void ADSGiftBox::OnComponentBeginOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);
	if (!IsValid(Character))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);
	check(UIManager);

	ItemListWidget = Cast<UDSItemList>(UIManager->PushContentToLayer(ListWidgetTag));

	if (IsValid(ItemListWidget))
	{
		DSEVENT_DELEGATE_BIND(OnUpdateItemWidget, ItemListWidget, &UDSItemList::AddItems);
		DSEVENT_DELEGATE_BIND(OnRemoveItemWidget, ItemListWidget, &UDSItemList::RemoveItem);
	}
	
	SetOwner(PlayerController);

}

void ADSGiftBox::OnComponentEndOverlap_Child(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);
	if (!IsValid(Character))
	{
		return;
	}

	//UI 띄우는 작업이 사라짐
	UDSUIManagerSubsystem* UIManager = UDSUIManagerSubsystem::Get(this);

	check(UIManager);

	UIManager->PopContentToLayer(ListWidgetTag);

	DSEVENT_DELEGATE_REMOVE(OnUpdateItemWidget, ItemListWidget);
	DSEVENT_DELEGATE_REMOVE(OnRemoveItemWidget, ItemListWidget);
	ItemListWidget = nullptr;
	SetOwner(nullptr);
}

