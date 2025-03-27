// Default
#include "Item/Weapon/DSWeapon.h"

// UE
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

ADSWeapon::ADSWeapon()
	: Super()
	, InputThreshold(0.f)
{
	Mesh = CreateDefaultSubobject< USkeletalMeshComponent>(TEXT("Mesh"));

	bReplicates = true;
	SetReplicateMovement(true);
}

void ADSWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitializeData();
	}
}

void ADSWeapon::InitializeData()
{
}

void ADSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSWeapon, InputThreshold);
}

