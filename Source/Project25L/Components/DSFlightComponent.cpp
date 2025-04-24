// Defualt
#include "Components/DSFlightComponent.h"

// UE
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

//Game
#include "Character/Characters/DSCharacter.h"
#include "Components/DSPlayerInputComponent.h"
#include "DSCharacterMovementComponent.h"
#include "DSLogChannels.h"
#include "System/DSEventSystems.h"

UDSFlightComponent::UDSFlightComponent()
	: Super()
	, Lean(0.f, 0.f)
	, CurrentState(EFlightState::None)
	, DefaultGravityScale(0.f)
	, LastVelocityRotation()
	, PreVelocityRotation()
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
	//else if(CurrentState == EFlightState::Hovering)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());

		if (false == IsValid(Character))
		{
			return;
		}
		float Velocity = Character->GetVelocity().Length();

		if (Velocity != 0.0f)
		{
			//움직이고 있다면,
			FRotator VelocityRotator = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity()); //실제 향하기 위한 방향

			FRotator ActorRotator = Character->GetActorRotation();

			LastVelocityRotation = FRotator(ActorRotator.Roll, VelocityRotator.Yaw ,ActorRotator.Pitch); //속도만큼 올라감을 표현하려는 거구나?
		}
		//Lean을 업데이트 한다. => Lean 과 회전은 일치하는 느낌 Turn in place 처럼 보간이 필요하기 때문에, Tick에서 수행해주어야한다.
		FRotator UpdateVelocityRotation = LastVelocityRotation - PreVelocityRotation;

		float YawDifferentVelocity = UpdateVelocityRotation.Yaw / DeltaTime; //매 프레임마다 얼마나 달라지는 지를 확인 - Yaw
		float PitchDifferentVelocity = UpdateVelocityRotation.Pitch / DeltaTime; //매 프레임마다 얼마나 달라지는 지를 확인 - Pitch

		PreVelocityRotation = UpdateVelocityRotation;

		YawDifferentVelocity = UKismetMathLibrary::MapRangeClamped(YawDifferentVelocity, -180.f, 180.f, -1.0f, 1.0f);
		PitchDifferentVelocity = UKismetMathLibrary::MapRangeClamped(PitchDifferentVelocity, -180.f, 180.f, -1.0f, 1.0f);

		Lean.X = FMath::FInterpTo(Lean.X, YawDifferentVelocity, DeltaTime, 5.0f);
		Lean.Y = FMath::FInterpTo(Lean.Y, PitchDifferentVelocity, DeltaTime, 5.0f);
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

	if (FlightVerticalImpulse.Contains(Direction))
	{
		Character->AddMovementInput(FVector(0.f, 0.f, FlightVerticalImpulse[Direction]));
	}
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


