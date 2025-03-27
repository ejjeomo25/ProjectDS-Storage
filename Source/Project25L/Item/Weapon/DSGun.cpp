// Default
#include "Item/Weapon/DSGun.h"

// UE
#include "Net/UnrealNetwork.h"

// Game
#include "GameData/DSWeaponData.h"
#include "GameData/DSGameDataSubsystem.h"


ADSGun::ADSGun()
	: Super()
	, BulletCount(0)
	, BulletSpread(0.f)
	, SpreadCoef(0.f)
{
}

void ADSGun::InitializeData()
{
	Super::InitializeData();

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData,EWeaponType>(EDataTableType::WeaponData, WeaponType);
	
	if (nullptr != WeaponData)
	{
		InputThreshold = WeaponData->InputThreshold;
		BulletCount = WeaponData->BulletCount;
		BulletSpread = WeaponData->BulletSpread;
		SpreadCoef = WeaponData->SpreadCoef;
	}
}

void ADSGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGun, BulletCount);
	DOREPLIFETIME(ADSGun, BulletSpread);
	DOREPLIFETIME(ADSGun, SpreadCoef);
}
