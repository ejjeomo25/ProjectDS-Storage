#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/PlayerController.h"

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

	void PlayDoorTransition();

public:
	/*Cheat*/
	UFUNCTION(Server, Unreliable)
	void ServerRPC_Cheat(const FString& Message);

	UFUNCTION(Server, Unreliable)
	void ServerRPC_CheatAll(const FString& Message);
	
	class UDSPlayerInputComponent* GetPlayerInputComponent() const { return DSPlayerInputComponent; }

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSPlayerInputComponent> DSPlayerInputComponent;

};
