// Default
#include "Character/DSArmedCharacter.h"

// UE
#include "Net/UnrealNetwork.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "GameData/DSWeaponData.h"
#include "Weapon/DSWeapon.h"


ADSArmedCharacter::ADSArmedCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsEquipped(false)
	, Weapon(nullptr)
{
}

void ADSArmedCharacter::BeginPlay()
{
	Super::BeginPlay();

	LoadWeapon();
}

void ADSArmedCharacter::LoadWeapon()
{
	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

	if (nullptr != WeaponData)
	{
		if (HasAuthority())
		{
			TSoftClassPtr<ADSWeapon> WeaponMesh = WeaponData->Weapon;

			UDSGameDataSubsystem::StreamableManager.RequestAsyncLoad(WeaponMesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([WeakPtr = TWeakObjectPtr<ADSArmedCharacter>(this), WeaponMesh]()
				{
					UWorld* World = WeakPtr->GetWorld();

					check(World);

					ADSArmedCharacter* Character = WeakPtr.Get();
					TSubclassOf<ADSWeapon> WeaponClass = WeaponMesh.Get();

					if (IsValid(Character))
					{
						FActorSpawnParameters Params;
						Params.Owner = Character;

						Character->Weapon = World->SpawnActor<ADSWeapon>(WeaponClass, FVector::ZeroVector, FRotator(0.f, 90.f, 0.f), Params);
						Character->Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_stow"));
					}
				}));
		}

		//애니메이션은 둘 다 가지고 온다.
		WeaponMontages.Add(EWeaponState::Equipped, WeaponData->EquipMontage.LoadSynchronous());
		WeaponMontages.Add(EWeaponState::Unequipped, WeaponData->UnEquipMontage.LoadSynchronous());
		WeaponMontages.Add(EWeaponState::Attack, WeaponData->AttackMontage.LoadSynchronous());
	}
}

void ADSArmedCharacter::Equip()
{
	EWeaponState EquipState = EWeaponState::Equipped;

	PlayAnimation(EquipState);

	//애니메이션을 실행한다.
	ServerRPC_EquipWeapon(EquipState);
}

void ADSArmedCharacter::UnEquip()
{
	EWeaponState EquipState = EWeaponState::Unequipped;

	PlayAnimation(EquipState);

	//애니메이션을 실행한다.
	ServerRPC_EquipWeapon(EquipState);
}

void ADSArmedCharacter::MoveEquip()
{

	EWeaponSocketType SocketType = EWeaponSocketType::Stow;

	if (bIsEquipped)
	{
		SocketType = EWeaponSocketType::Stow;
	}
	else
	{
		SocketType = EWeaponSocketType::Equipped;
	}

	if (IsValid(Weapon))
	{
		if (SocketName.Contains(SocketType))
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName[SocketType]);
		}
	}
	
	bIsEquipped = !bIsEquipped;
}

void ADSArmedCharacter::PlayAnimation(EWeaponState WeaponState)
{
	if (false == WeaponMontages.Contains(WeaponState))
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(WeaponMontages[WeaponState]))
		{
			//현재 애니메이션 몽타주가 실행중임으로 리턴한다.
			return;
		}
	}
	if (IsValid(WeaponMontages[WeaponState]))
	{
		PlayAnimMontage(WeaponMontages[WeaponState]);
	}
}

float ADSArmedCharacter::GetInputThreshold()
{
	if (IsValid(Weapon))
	{
		float InputThreshold = Weapon->GetInputThreshold();

		//단발인가? InputThreshold		
		//스킬인가? InputThreshold*2.f;

		return InputThreshold;
	}

	return 0.0f;
}

void ADSArmedCharacter::ServerRPC_EquipWeapon_Implementation(EWeaponState EquipState)
{
	UWorld* World = GetWorld();

	check(World);

	PlayAnimation(EquipState);

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (IsValid(PlayerController) && GetController() != PlayerController)
		{
			if (false == PlayerController->IsLocalController())
			{
				ADSArmedCharacter* OtherPlayer = Cast<ADSArmedCharacter>(PlayerController->GetPawn());
				if (IsValid(OtherPlayer))
				{
					OtherPlayer->ClientRPC_EquipWeapon(this, EquipState);
				}
			}
		}
	}
}

void ADSArmedCharacter::ClientRPC_EquipWeapon_Implementation(ADSArmedCharacter* Character, EWeaponState EquipState)
{
	if (IsValid(Character))
	{
		Character->PlayAnimation(EquipState);
	}
}

void ADSArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSArmedCharacter, Weapon);
}
