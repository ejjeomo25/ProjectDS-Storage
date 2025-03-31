#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/PlayerController.h"

// Game
#include "GameData/DSEnums.h"

//UHT
#include "DSPlayerController.generated.h"

class UDSPlayerInputComponent;

UCLASS()
class PROJECT25L_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ADSPlayerController();

public:
	void SetUIFocusMode();
	void SetGameFocusMode();
	
	ECharacterType GetCharacterType() { return CharacterType; }

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_Cheat(const FString& Message);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_CheatAll(const FString& Message);

protected:
	
	/*현재 CharacterType은 블루프린트로 넣는 용도이지만, 추후에 선택에 의해 변경한 Transient 로 변경 예정*/
	UPROPERTY(EditAnywhere, category = CharacterType)
	ECharacterType CharacterType;

};
