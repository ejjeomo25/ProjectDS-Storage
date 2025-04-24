// Default
#include "Weapon/DSGun.h"

// UE
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Game
#include "Character/DSArmedCharacter.h"
#include "Components/DSStatComponent.h"
#include "GameData/DSWeaponData.h"
#include "System/DSGameDataSubsystem.h"
#include "System/DSGameUtils.h"


ADSGun::ADSGun()
	: Super()
	, GravityGunTargets()
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


void ADSGun::AttackPrimarySkill(const float& AutoAimAngle, const float& AttackDistance) const
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	FVector StartLoc = GetMuzzleSocketLocation();
	FVector TargetLoc = GetFinalDestination(AutoAimAngle, 400.f);

	//애니메이션을 실행한다.
	Character->PlayAnimation(EWeaponState::Attack);

	//로컬이 서버인 경우 == 리슨서버는 아래 RPC를 실행하지 않는다.
	ServerRPC_PrimaryAttack(StartLoc, TargetLoc);
}

void ADSGun::AttackSkill1(const FVector& TargetLocation, const float& AttackRadius, const float& Impulse)
{
	DrawDebugCircle(
		GetWorld(),
		TargetLocation,                // 중심 좌표 (FVector)
		AttackRadius,            // 반지름 (float)
		32,                    // 세그먼트 수 (원 매끄럽게 할수록 높임)
		FColor::Cyan,          // 색상
		false,                 // 영구 표시 여부
		5.0f,                  // 지속 시간
		0,                     // 깊이 우선 순위
		1.0f,                  // 선 두께
		FVector(1, 0, 0),        // Y축 회전 기준 벡터 (보통 X축)
		FVector(0, 1, 0),        // Z축 회전 기준 벡터 (보통 Y축)
		false                  // 3D 회전 원 여부 (false면 평면 원)
	);

	ServerRPC_SkillAttack(TargetLocation, AttackRadius, Impulse);
}

void ADSGun::AttackSkill2(const float& AutoAimAngle, const float& AttackDistance, const float& AttackRadius)
{
	DS_NETLOG(DSSkillLog, Log, TEXT(""));
}

FVector ADSGun::GetMuzzleSocketLocation() const
{
	return  Mesh->GetSocketLocation(TEXT("MuzzleFlush"));
}

FVector ADSGun::GetFinalDestination(const float& AimAngle, const float& SkillDistance) const
{
	if (false == IsValid(Mesh))
	{
		return FVector();
	}

	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return  FVector();
	}

	// 시각적 표현, 나이아가라
	// 목표지점까지는 나오고 => 사정거리 내에 적이 있는지 확인하고, 가까운 적한테 쏜다.
	const FVector& TargetLoc = GetAutoTargetingLocation(AimAngle, SkillDistance);

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
	const FVector& SpreadTargetLoc = StartLoc + GetSpreadShotTargetLocation(TargetDir) * SkillDistance;
	return SpreadTargetLoc;
}

void ADSGun::EndPrimaryAttack() const
{
	ShotsFired = 0.0f;
}

void ADSGun::EndAttackSkill1()
{
	ServerRPC_EndAttackSkill1();
}

FVector ADSGun::GetSpreadShotTargetLocation(const FVector& TargetDir) const
{
	UWorld* World = GetWorld();

	check(World);

	// Timeline의 가장 큰 Time을 가지고와서
	float MinTime, MaxTime;
	YawRecoil->GetTimeRange(MinTime, MaxTime);
	
	// 실제 누르는 횟수를 반영
	ShotsFired++;

	// 시간초를 반영해서
	float TimeSinceLastShot = FMath::Fmod(ShotsFired * InputThreshold, MaxTime);

	// Value값을 가져온다.
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
void ADSGun::ServerRPC_SkillAttack_Implementation(const FVector& TargetLoc, const float& SkillDistance, const float& ZImpulse)
{
	ADSArmedCharacter* Character = Cast<ADSArmedCharacter>(GetOwner());

	if (false == IsValid(Character))
	{
		return;
	}

	UWorld* World = GetWorld();

	check(World);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); /*아이템 채널로 변경 해야 함!!*/
	IgnoreActors.Add(Character); //같은 팀은 제외해야한다.!!!

	//오버랩을 생성한다.
	bool Result = UKismetSystemLibrary::SphereOverlapActors(World, TargetLoc, SkillDistance, ObjectTypes, nullptr, IgnoreActors, OutActors);

	//해당 오버랩에 들어있는 적에게 중력을 끄고, AddForce 적용한다.
	if (Result)
	{
		for (AActor* Target : OutActors)
		{
			//나중에 팀플은 제외한다.!!!
			ACharacter* TargetCharacter = Cast<ACharacter>(Target);

			if (IsValid(TargetCharacter))
			{
				UCharacterMovementComponent* CharacterMovement = TargetCharacter->GetCharacterMovement();

				if (IsValid(CharacterMovement))
				{
					CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying);
					CharacterMovement->AddImpulse(FVector(0.0f, 0.f, ZImpulse), true); //현재 ZImpulse 값이 끝나면, 바로 멈춤. 
					CharacterMovement->AirControl = 0.1f; // 공중에서 약간의 제어만 가능, 0: 공중에서 방향전환 안됨 / 1: 공중에서도 방향을 자유롭게 바꿀 수 있음.
					CharacterMovement->BrakingDecelerationFlying = 100.0f; // 비행 중일 때 감속하는 정도로, 입력을 멈췄을 때 관성없이 빠르게 속도가 줄어들길 원하면 이 값이 높음.
					GravityGunTargets.Add(Target);
				}
			}
		}
	}
}

void ADSGun::ServerRPC_EndAttackSkill1_Implementation()
{
	for (const auto Target : GravityGunTargets)
	{
		//나중에 팀플은 제외한다.!!!
		ACharacter* TargetCharacter = Cast<ACharacter>(Target);

		if (IsValid(TargetCharacter))
		{
			UCharacterMovementComponent* CharacterMovement = TargetCharacter->GetCharacterMovement();

			if (IsValid(CharacterMovement))
			{
				CharacterMovement->SetMovementMode(EMovementMode::MOVE_Falling);
				CharacterMovement->GravityScale = 2.5f;
			}
		}
	}
}
void ADSGun::ServerRPC_PrimaryAttack_Implementation(const FVector& StartLoc, const FVector& TargetLoc) const
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
		TargetLoc,           // 끝 지점 (FVector)
		FColor::Green,           // 색상
		false,                 // 영구 표시 여부 (true면 계속 유지)
		5.0f,                  // 지속 시간 (초 단위)
		0,                     // 깊이 우선 순위
		2.0f                   // 선 두께
	);

	// 실제 라인트레이스 진행
	UWorld* World = GetWorld();

	check(World);

	bool bResult = false;
	FHitResult OutHitResult;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GunLineTrace), false, Character); //식별자 
	FCollisionResponseParams ResponseParams;

	bResult = World->LineTraceSingleByChannel(
		OutHitResult,
		StartLoc,
		TargetLoc,
		ECollisionChannel::ECC_Pawn,
		Params,
		ResponseParams
		);

	if (bResult)
	{
		ADSCharacterBase* TargetCharacter = Cast<ADSCharacterBase>(OutHitResult.GetActor());

		if (IsValid(TargetCharacter))
		{
			FDSDamageEvent DamageEvent;
			DamageEvent.DamageType = EDamageType::BaseAttack;
			TargetCharacter->TakeFinalDamage(50.f, DamageEvent,Character->GetController(),Character);
		}

	}

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