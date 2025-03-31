#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Components/ActorComponent.h"
#include "GameData/DSEnums.h"

// UHT
#include "DSFlightComponent.generated.h"

// Delegate

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFlightStateChanged, EFlightState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT25L_API UDSFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDSFlightComponent();

	void BeginFlight();
	void EndFlight();
	bool EnableFlying() const;
	EFlightState GetFlightState() const { return CurrentState;}

public:
	FOnFlightStateChanged OnFlightStateChanged;

protected:
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void SetFlightState(EFlightState NewState);

	void Dodge();
	void Hovering();

private:
	UFUNCTION(Server, Unreliable)
	void ServerRPC_BeginFlight();
	UFUNCTION(Server, Unreliable)
	void ServerRPC_EndFlight();

	void MoveFlight(EFlightState Direction);
	void OnChangeFlightBegin();
	void OnChangeFlightEnd();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlightDirection", Meta = (AllowPrivateAccess = "true"))
	TMap<EFlightState, float> FlightVerticalImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlightAnimation", Meta = (AllowPrivateAccess = "true"))
	TMap<EFlightState,UAnimMontage*> FlightAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCoefficient", Meta = (AllowPrivateAccess = "true"))
	float GravityCoefficient;

	UPROPERTY(Replicated, Transient)
	EFlightState CurrentState;

	float DefaultGravityScale ;
};
