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
		FStreamableManager StreamableManager;

		if (HasAuthority())
		{
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

						Character->Weapon = World->SpawnActor<ADSWeapon>(WeaponClass, FVector::ZeroVector, FRotator(0.f, 90.f, 0.f), Params);
						Character->Weapon->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_stow"));
					}
				}));
		}

		//애니메이션은 둘 다 가지고 온다.
		EquipMontages.Add(EWeaponEquipState::Equipped, WeaponData->EquipMontage.LoadSynchronous());
		EquipMontages.Add(EWeaponEquipState::Unequipped, WeaponData->UnEquipMontage.LoadSynchronous());
	}
}

void ADSArmedCharacter::Equip()
{
	EWeaponEquipState EquipState = EWeaponEquipState::Equipped;


	//*****************코드 리뷰 : PlayMontage 부분 함수로 빼기 ****************************//
	if (EquipMontages.Contains(EquipState) == false)
	{
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(EquipMontages[EquipState]))
		{
			//현재 애니메이션 몽타주가 실행중임으로 리턴한다.
			return;
		}
	}
	if (IsValid(EquipMontages[EquipState]))
	{
		PlayAnimMontage(EquipMontages[EquipState]);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	
	//애니메이션을 실행한다.
	ServerRPC_EquipWeapon(EquipState);
}

void ADSArmedCharacter::UnEquip()
{
	EWeaponEquipState EquipState = EWeaponEquipState::Unequipped;

	if (EquipMontages.Contains(EquipState) == false)
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		if (AnimInstance->Montage_IsPlaying(EquipMontages[EquipState]))
		{
			//현재 애니메이션 몽타주가 실행중임으로 리턴한다.
			return;
		}
	}

	if (IsValid(EquipMontages[EquipState]))
	{
		PlayAnimMontage(EquipMontages[EquipState]);
	}
	//애니메이션을 실행한다.
	ServerRPC_EquipWeapon(EquipState);
}

void ADSArmedCharacter::MoveEquip()
{

	//*****************코드 리뷰 : 변수로 빼기 EditAnywhere ****************************//
	if (bIsEquipped)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_stow"));
	}
	else
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ik_hand_r"));
	}

	bIsEquipped = !bIsEquipped;
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

void ADSArmedCharacter::ServerRPC_EquipWeapon_Implementation(EWeaponEquipState EquipState)
{
	UWorld* World = GetWorld();

	check(World);

	if (EquipMontages.Contains(EquipState))
	{
		PlayAnimMontage(EquipMontages[EquipState]);
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (IsValid(PlayerController) && GetController() != PlayerController)
		{
			//*****************코드 리뷰 : false 처리 ****************************//
			if (!PlayerController->IsLocalController())
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

void ADSArmedCharacter::ClientRPC_EquipWeapon_Implementation(ADSArmedCharacter* Character, EWeaponEquipState EquipState)
{
	if (IsValid(Character))
	{
		if (EquipMontages.Contains(EquipState))
		{
			Character->PlayAnimMontage(EquipMontages[EquipState]);
		}
	}
}

void ADSArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSArmedCharacter, Weapon);
}
