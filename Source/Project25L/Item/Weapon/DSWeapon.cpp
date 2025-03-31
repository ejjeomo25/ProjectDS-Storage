// Default
#include "Item/Weapon/DSWeapon.h"

// UE
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Game
#include "GameData/DSGameDataSubsystem.h"
#include "GameData/Skill/DSSkillAttribute.h"
#include "Player/DSPlayerController.h"
#include "System/DSGameUtils.h"

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

FVector ADSWeapon::GetAutoTargetingLocation(const float& AimAngle, const float& AttackRadius) const
{
	
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if (IsValid(Owner) == false)
	{
		return FVector();
	}

	//MuzzleFlush 는 Gun에서만 해당되기 때문에 Socket name 변경한다. 원거리에서 사용할 수 있는 소켓이름으로.
	const FVector& Location = Mesh->GetSocketLocation(TEXT("MuzzleFlush"));

	// 0.f 가까우면 플레이어가 바라보는 방향으로, 스킬 Attack Distance 만큼 전달, 가장 기본적인 공격 위치 지정
	FVector TargetLoc = Location + Character->GetActorForwardVector() * AttackRadius;

	if (FMath::IsNearlyZero(AimAngle) == false)
	{
		UWorld* World = GetWorld();
		
		check(World);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> IgnoreActors;
		TArray<AActor*> OutActors;

		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
		//캐릭터와 같은 팀일 경우 Ignore 시킬 예정
		IgnoreActors.Add(Character);
		
		bool Result = UKismetSystemLibrary::SphereOverlapActors(World, Location, AttackRadius, ObjectTypes, nullptr, IgnoreActors, OutActors);
		
		DrawDebugCircle(
			GetWorld(),
			Location,                // 중심 좌표 (FVector)
			AttackRadius,            // 반지름 (float)
			32,                    // 세그먼트 수 (원 매끄럽게 할수록 높임)
			FColor::Blue,          // 색상
			false,                 // 영구 표시 여부
			5.0f,                  // 지속 시간
			0,                     // 깊이 우선 순위
			1.0f,                  // 선 두께
			FVector(1, 0, 0),        // Y축 회전 기준 벡터 (보통 X축)
			FVector(0, 1, 0),        // Z축 회전 기준 벡터 (보통 Y축)
			false                  // 3D 회전 원 여부 (false면 평면 원)
		);

		if (Result)
		{
			// Overlap에서 나온 모든 적 NPC를 가지고 온다
				// 적이 시야범위에 드는지 확인한다.	
					//시야범위에 있는 적 중에서 가장 가까운 적을 목표지점으로 삼는다.
			float MinDistance = AttackRadius; //갈 수 있는 최대 거리임.

			for (const AActor* OutActor : OutActors)
			{
				bool bIsInFOV = UDSGameUtils::IsWithinCharacterFOV(Character, OutActor, AimAngle);

				if (bIsInFOV)
				{
					const FVector& OtherLocation = OutActor->GetActorLocation();

					float Distance = FMath::Abs(FVector::Distance(Location, OtherLocation));

					if (Distance <= MinDistance)
					{
						TargetLoc = OtherLocation;
						MinDistance = Distance;
					}
				}
			}
		}
	}
	return TargetLoc;
}

FVector ADSWeapon::GetSpreadShotTargetLocation(const FVector& TargetDir, float SpreadCoef) const
{
	//SpreadCoef를 기준으로 -SpreadCoef, +SpreadCoef 를 만든다. => 중복은 허용하지 않는다.
	int32 YawIdx = UDSGameUtils::GetSpreadOffsetIdx();
	int32 PitchIdx = UDSGameUtils::GetSpreadOffsetIdx();

	float Yaw = UDSGameUtils::GetSpreadOffset(YawIdx);
	float Pitch = UDSGameUtils::GetSpreadOffset(PitchIdx);

	const FRotator& SpreadOffsetRotator = FRotator(Pitch, Yaw, 0.f);
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

void ADSWeapon::InitializeData()
{
}

void ADSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADSWeapon, InputThreshold);
}

void ADSWeapon::PrimaryAttack(const float& AimAngle, const float& SkillRadius) const
{
}

