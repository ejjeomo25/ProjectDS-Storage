// Defualt
#include "Components/DSFlightComponent.h"

// UE
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

//Game
#include "System/DSEventSystems.h"
#include "DSLogChannels.h"
#include "Character/Characters/DSCharacter.h"
#include "Components/DSPlayerInputComponent.h"
#include "DSCharacterMovementComponent.h"

UDSFlightComponent::UDSFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

// Called when the game starts
void UDSFlightComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetFlightState(EFlightState::None);
	DSEVENT_DELEGATE_BIND(OnFlightStateChanged, this, &UDSFlightComponent::SetFlightState);
}

void UDSFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrentState == EFlightState::Down)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (false == IsValid(Character))
		{
			return;
		}

		UDSCharacterMovementComponent* MovementComponent = Cast< UDSCharacterMovementComponent>(Character->GetCharacterMovement());

		if (false == IsValid(MovementComponent))
		{
			return;
		}

		if (true == MovementComponent->CanLand())
		{
			SetFlightState(EFlightState::End);
		}
	}
}

void UDSFlightComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDSFlightComponent, CurrentState);
}

void UDSFlightComponent::BeginFlight()
{
	ServerRPC_BeginFlight();
}

void UDSFlightComponent::EndFlight()
{
	ServerRPC_EndFlight();

	// 쿨타임이 있다면 Lock으로 바껴야 함
	SetFlightState(EFlightState::None);

	ADSCharacter* Character = Cast< ADSCharacter>(GetOwner());
	DSEVENT_DELEGATE_INVOKE(Character->GetPlayerInputComponent()->OnInputMappingChangedEvent, EInputMappingContextType::DefaultIMC);
}

bool UDSFlightComponent::EnableFlying() const
{
	return CurrentState == EFlightState::Locked ? false : true;
}

void UDSFlightComponent::Dodge()
{
	// 인풋 방향에 따라, Animation이 재생될 듯
	// BlendSpace로
}

void UDSFlightComponent::Hovering()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->Velocity.Z = 0;
	}
}

void UDSFlightComponent::MoveFlight(EFlightState Direction)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	Character->AddMovementInput(FVector(0.f, 0.f, FlightVerticalImpulse[Direction]));
}

void UDSFlightComponent::OnChangeFlightBegin()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		DefaultGravityScale = MovementComponent->GravityScale;
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
		MovementComponent->GravityScale *= GravityCoefficient;
	}
}

void UDSFlightComponent::OnChangeFlightEnd()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		MovementComponent->GravityScale = DefaultGravityScale;
	}
}

void UDSFlightComponent::ServerRPC_BeginFlight_Implementation()
{
	OnChangeFlightBegin();
}
void UDSFlightComponent::ServerRPC_EndFlight_Implementation()
{
	OnChangeFlightEnd();
}

void UDSFlightComponent::SetFlightState(EFlightState NewState)
{
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EFlightState::Begin: BeginFlight(); break;
	case EFlightState::Up: MoveFlight(EFlightState::Up); break;
	case EFlightState::Down: MoveFlight(EFlightState::Down); break;
	case EFlightState::Hovering: Hovering(); break;
	case EFlightState::Dodge: Dodge(); break;
	case EFlightState::End: EndFlight(); break;
	}
}


