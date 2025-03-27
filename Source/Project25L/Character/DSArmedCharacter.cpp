// Default
#include "Character/DSArmedCharacter.h"

// UE
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"

// Game
#include "GameData/DSGameDataSubsystem.h"
#include "GameData/DSWeaponData.h"
#include "Item/Weapon/DSWeapon.h"


ADSArmedCharacter::ADSArmedCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Weapon(nullptr)
{
}

void ADSArmedCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		LoadWeapon();
	}
}

void ADSArmedCharacter::LoadWeapon()
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

	if (nullptr != WeaponData)
	{
		FStreamableManager StreamableManager;

		TSoftClassPtr<ADSWeapon> WeaponMesh = WeaponData->Weapon;

		StreamableManager.RequestAsyncLoad(WeaponMesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakPtr = TWeakObjectPtr<ADSArmedCharacter>(this), WeaponMesh]()
			{
				UWorld* World = WeakPtr->GetWorld();

				check(World);

				ADSArmedCharacter* Character = WeakPtr.Get();
				TSubclassOf<ADSWeapon> WeaponClass = WeaponMesh.Get();

				if (IsValid(Character))
				{
					FActorSpawnParameters Params;
					Params.Owner = Character;

					Character->Weapon = World->SpawnActor<ADSWeapon>(WeaponClass,FVector::ZeroVector, FRotator(0.f,90.f,-90.f), Params);
					Character->Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ik_hand_gun"));
				}
			}));
	}
}

void ADSArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSArmedCharacter, Weapon);
}
