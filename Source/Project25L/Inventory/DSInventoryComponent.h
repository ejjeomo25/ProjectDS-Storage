#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Components/PawnComponent.h"

// UHT
#include "DSInventoryComponent.generated.h"

class IDSItem;

UCLASS()
class PROJECT25L_API UDSInventoryComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UDSInventoryComponent(const FObjectInitializer& ObjectInitializer);

	// 파밍 키를 눌렀을 때 해당 Item의 Type과 Count를 전달한다.
	void StoreItems(int32 ItemID, int32 Count);

	// 키를 사용해서 아이템 사용을 눌렀을 때 실제로 사용 한다.
	// 현재 아이템 타입이 있는지 없는지를 검사를 수행한다.
	void UseItem(int32 ItemID);

	// N개 이상의 아이템을 사용하는 경우의 수
	void UseItem(int32 ItemID, int32 ItemCount);

	// 아이템 사용시 실제 아이템 스탯이 적용된다.
	void OnItemUsed(int32 ItemID);
protected:

	// 아이템 줍는 함수
	// 서버에게 전달한다.
	// Validate 검사를 수행하지만, return false를 하지 않고, 아이템 개수를 롤백하는 방법을 선택한다.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_StoreItems(int32 ItemID, int Count, int ClientItemCount);

	// 클라이언트와 서버의 아이템 개수가 다를 경우, 롤백한다.
	UFUNCTION(Client, Reliable)
	void ClientRPC_RollbackItems(int32 ItemID, int Count);

protected:

	// 아이템 사용 함수
	UFUNCTION(Server, Reliable)
	void ServerRPC_UseItem(int32 ItemID, int ClientItemCount);

protected:

	UPROPERTY(Transient)
	TMap<int32, int32> PersonalInventory;
};
