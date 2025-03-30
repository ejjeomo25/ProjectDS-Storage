// Default
#include "Item/Weapon/DSGun.h"

// UE
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// Game
#include "GameData/DSWeaponData.h"
#include "GameData/DSGameDataSubsystem.h"
#include "System/DSGameUtils.h"

#include "DSLogChannels.h"

ADSGun::ADSGun()
	: Super()
	, SpreadCoef(0.f)
{
}

void ADSGun::BeginPlay()
{
	Super::BeginPlay();

	//상대 경로가 아닌 절대 경로로 지정해주어야 한다.
	FString Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames/") + FilePath);
	UDSGameUtils::LoadSpreadOffset(Path, SpreadCoef);
}

void ADSGun::InitializeData()
{
	Super::InitializeData();

	UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

	check(DataManager);

	FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

	if (nullptr != WeaponData)
	{
		InputThreshold = WeaponData->InputThreshold;
		SpreadCoef = WeaponData->SpreadCoef;
	}
}

void ADSGun::PrimaryAttack(const float& AimAngle, const float& SkillRadius) const
{

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (IsValid(Character) == false)
	{
		return;
	}

	if (IsValid(Mesh) == false)
	{
		return;
	}

	if (Character->IsLocallyControlled())
	{
		// 시각적 표현, 나이아가라
		// 목표지점까지는 나오고 => 사정거리 내에 적이 있는지 확인하고, 가까운 적한테 쏜다.
		const FVector& TargetLoc = GetAutoTargetingLocation(AimAngle, SkillRadius);

		const FVector& StartLoc = Mesh->GetSocketLocation(TEXT("MuzzleFlush"));

		DrawDebugLine(
			GetWorld(),
			StartLoc,         // 시작 지점 (FVector)
			TargetLoc,           // 끝 지점 (FVector)
			FColor::Red,           // 색상
			false,                 // 영구 표시 여부 (true면 계속 유지)
			5.0f,                  // 지속 시간 (초 단위)
			0,                     // 깊이 우선 순위
			2.0f                   // 선 두께
		);


		const FVector& TargetDir = (TargetLoc - StartLoc).GetSafeNormal();

		//TargetLoc 으로 자연스럽게 회전한다. Turn To Target
		FVector LookVector = TargetDir;

		// Yaw값은 회전에서 제외 시켜준다.
		
		//회전 로직


		const FVector& SpreadTargetLoc = StartLoc + GetSpreadShotTargetLocation(TargetDir, SpreadCoef) * SkillRadius;

		DrawDebugLine(
			GetWorld(),
			StartLoc,         // 시작 지점 (FVector)
			SpreadTargetLoc,           // 끝 지점 (FVector)
			FColor::Green,           // 색상
			false,                 // 영구 표시 여부 (true면 계속 유지)
			5.0f,                  // 지속 시간 (초 단위)
			0,                     // 깊이 우선 순위
			2.0f                   // 선 두께
		);
		DS_LOG(DSSkillLog, Log, TEXT("Auto Targeting Angle %lf SkillRadius %lf Monster %s"), AimAngle, SkillRadius, *TargetLoc.ToString(), *SpreadTargetLoc.ToString());

		//로컬이 서버인 경우 == 리슨서버는 아래 RPC를 실행하지 않는다.
		if (HasAuthority() == false)
		{
			ServerRPC_PrimaryAttack();
		}
	}
}

void ADSGun::ServerRPC_PrimaryAttack_Implementation() const
{
	if (!IsValid(Mesh))
		return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (IsValid(Character) == false)
	{
		return;
	}

	// 목표지점까지 나오고, 사정 거리 내에 적이 있는지 확인하고 가까운 적한테 쏜다.
}

void ADSGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGun, SpreadCoef);
}