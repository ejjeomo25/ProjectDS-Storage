// Default
#include "Weapon/DSGun.h"

// UE
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// Game
#include "DSLogChannels.h"
#include "System/DSGameDataSubsystem.h"
#include "Character/DSArmedCharacter.h"
#include "GameData/DSWeaponData.h"
#include "System/DSGameUtils.h"

ADSGun::ADSGun()
	: Super()
	, SpreadCoef(0.f)
	, ShotsFired(0)
	, FilePath(TEXT("SpreadOffsetFile.txt"))
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

	if (HasAuthority())
	{
		UDSGameDataSubsystem* DataManager = UDSGameDataSubsystem::Get(this);

		check(DataManager);

		FDSWeaponData* WeaponData = DataManager->GetDataRowByEnum<FDSWeaponData, EWeaponType>(EDataTableType::WeaponData, WeaponType);

		if (nullptr != WeaponData)
		{
			InputThreshold = WeaponData->InputThreshold;
			SpreadCoef = WeaponData->SpreadCoef;
		}
	}

	/*Recoil Curve Timeline 연결*/
	YawRecoil = UDSGameDataSubsystem::StreamableManager.LoadSynchronous(YawRecoilClass);
	PitchRecoil = UDSGameDataSubsystem::StreamableManager.LoadSynchronous(PitchRecoilClass);
}

void ADSGun::PrimaryAttack(const float& AimAngle, const float& SkillRadius) const
{
	if (false == IsValid(Mesh))
		return;

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

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

	if (bUseAutoTargeting)
	{
		// 오토 타겟이 되어있다면.
		// Yaw값은 회전에서 제외 시켜준다.

		// 회전 로직
	}
	
	//Yaw와 Pitch에 대한 결과값을 ServerRPC에게 전송해야한다.
	const FVector& SpreadTargetLoc = StartLoc + GetSpreadShotTargetLocation(TargetDir) * SkillRadius;

	//애니메이션을 실행한다.
	Character->PlayAnimation(EWeaponState::Attack);

	//로컬이 서버인 경우 == 리슨서버는 아래 RPC를 실행하지 않는다.
	ServerRPC_PrimaryAttack(StartLoc, SpreadTargetLoc);
}

void ADSGun::EndPrimaryAttack()
{
	ShotsFired = 0.0f;
}

FVector ADSGun::GetSpreadShotTargetLocation(const FVector& TargetDir) const
{
	
	/*
	- 타임 라인을 반영한다.
	- 실제 초를 제공하기 위해서 GetWorld()->GetDeltaTime() 를 가져와서 사용
	- 연사시 더 튕김을 더 가해줌
	*/
	UWorld* World = GetWorld();

	check(World);

	float MinTime, MaxTime;
	YawRecoil->GetTimeRange(MinTime, MaxTime);
	
	ShotsFired++;

	float TimeSinceLastShot = FMath::Fmod(ShotsFired * InputThreshold, MaxTime);

	float YawRecoilValue= YawRecoil->GetFloatValue(TimeSinceLastShot);
	float PitchRecoilValue = PitchRecoil->GetFloatValue(TimeSinceLastShot);

	const FRotator& RecoilRotator = FRotator(PitchRecoilValue, YawRecoilValue, 0.f);

	float Yaw = UDSGameUtils::GetSpreadOffset();
	float Pitch = UDSGameUtils::GetSpreadOffset();

	const FRotator& SpreadOffsetRotator = FRotator(Pitch, Yaw, 0.f) + RecoilRotator;
	const FRotator& BaseRot = TargetDir.Rotation();

	//짐벌락 현상을 없애기 위해 Quternion으로 계산
	const FQuat& AQuat = FQuat(SpreadOffsetRotator);
	const FQuat& BQuat = FQuat(BaseRot);

	// 두 곱셈을 통해서 Yaw/Pitch 값 반영
	FRotator FinalRotator = FRotator(BQuat * AQuat);;

	// 최종 방향 벡터
	FVector FinalDirection = FinalRotator.Vector();

	return FinalDirection;
}

void ADSGun::ServerRPC_PrimaryAttack_Implementation(const FVector& StartLoc, const FVector& TargetDir) const
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	// 목표지점까지 나오고, 사정 거리 내에 적이 있는지 확인하고 가까운 적한테 쏜다.
	DrawDebugLine(
		GetWorld(),
		StartLoc,         // 시작 지점 (FVector)
		TargetDir,           // 끝 지점 (FVector)
		FColor::Green,           // 색상
		false,                 // 영구 표시 여부 (true면 계속 유지)
		5.0f,                  // 지속 시간 (초 단위)
		0,                     // 깊이 우선 순위
		2.0f                   // 선 두께
	);

	// 실제 라인트레이스 진행
	
	// 리슨서버 제외 애니메이션 실행
	if (false == Character->HasAuthority() && Character->IsLocallyControlled())
	{
		Character->PlayAnimation(EWeaponState::Attack);

		//모든 클라이언트를 가지고 와서 애니메이션 실행을 요청함.
	}
}


void ADSGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSGun, SpreadCoef);
}