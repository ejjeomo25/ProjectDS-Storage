//Default
#include "Character/DSCharacterMovementComponent.h"

//UE

//Game
#include "Character/DSCharacter.h"
#include "DSLogChannels.h"


UDSCharacterMovementComponent::UDSCharacterMovementComponent()
{
	bChangeSpeed = false;
}

void UDSCharacterMovementComponent::SetSpeedCommand(ESpeedType TargetWalkSpeed)
{
	bChangeSpeed = true;
	CurrentSpeedType = TargetWalkSpeed;
}


FNetworkPredictionData_Client* UDSCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UDSCharacterMovementComponent* MutableThis = const_cast<UDSCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FDSNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}


void UDSCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (bChangeSpeed)
	{
		bChangeSpeed = false;
	}
}

float UDSCharacterMovementComponent::GetMaxSpeed() const
{
	if (SpeedMode.Num() == 0)
	{
		return Super::GetMaxSpeed();
	}
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return IsCrouching() ? MaxWalkSpeedCrouched : SpeedMode[CurrentSpeedType];
	case MOVE_Falling:
		return SpeedMode[CurrentSpeedType];
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UDSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	uint8 ExtractedSpeed = Flags & 0xF0;
	ESpeedType SpeedType = static_cast<ESpeedType>(ExtractedSpeed);

	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		if (SpeedMode.Contains(SpeedType))
		{
			CurrentSpeedType = SpeedType;
		}
	}
}


FDSNetworkPredictionData_Client_Character::FDSNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	:FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr FDSNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FDSSavedMove_Character());
}



void FDSSavedMove_Character::Clear()
{
	FSavedMove_Character::Clear();
	bChangeSpeed = false;
	
}

void FDSSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	FSavedMove_Character::SetInitialPosition(Character);

	UDSCharacterMovementComponent* DSMovement = Cast<UDSCharacterMovementComponent>(Character->GetCharacterMovement());

	if (DSMovement)
	{
		bChangeSpeed = DSMovement->bChangeSpeed;
		SavedSpeedType = DSMovement->CurrentSpeedType;
	}
}

uint8 FDSSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	Result |= (static_cast<uint8>(SavedSpeedType) & 0xF0);

	return Result;
}
