// Default
#include "System/DSGameUtils.h"

// UE
#include "GameFramework/PlayerController.h"

// Game
#include "System/DSGameInstance.h"
#include "Character/DSCharacter.h"

UDSGameUtils::UDSGameUtils()
	: Super()
{
}

UDSGameUtils* UDSGameUtils::Get(UObject* Object)
{
	UWorld* World = Object->GetWorld();

	check(World);

	UDSGameInstance* GameInstance = Cast<UDSGameInstance>(World->GetGameInstance());

	check(GameInstance);

	return GameInstance->GameUtils;
}

bool UDSGameUtils::IsWithinCharacterFOV(const ADSCharacter* Character, const AActor* Target, float AnchorAngle)
{
	//만약에 캐릭터라면 캐릭터가 바라보는 forward vector 가져온다.
	const FVector& CharacterForwardVec = Character->GetActorForwardVector();
	FVector CharacterLoc = Character->GetActorLocation(); //Z값을 고려하지 않는다.
	FVector Loc = Target->GetActorLocation();

	// Z를 동일하게 맞춰준다.
	// 이유: Z값을 고려할 경우, 내적에 해당하는 방향 값이 달라진다.
	CharacterLoc.Z = 0;
	Loc.Z = 0;

	// 캐릭터를 기준으로 방향을 알아냄
	FVector Dir = Loc - CharacterLoc;
	Dir.Normalize(); // Character가 바라보는 forward vector 값이 노멀된 값이기 때문에 노멀라이즈 해준다.

	// 두 벡터간의 내적과 acos을 사용해서 거리를 알 수 있다.
	float Dot = FVector::DotProduct(CharacterForwardVec, Dir);
	Dot = FMath::Clamp(Dot, -1.0f, 1.0f);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	return abs(Angle) <= AnchorAngle;
}

ACharacter* UDSGameUtils::GetCharacter(const APlayerController* PlayerController)
{
	ACharacter* Character = PlayerController->GetPawn<ACharacter>();

	if (IsValid(Character))
	{
		return Character;
	}
	return nullptr;
}

APlayerController* UDSGameUtils::GetPlayerController(const ACharacter* Character)
{
	APlayerController* PlayerController = Character->GetController<APlayerController>();

	if (IsValid(PlayerController))
	{
		return PlayerController;
	}

	return nullptr;
}

uint32 UDSGameUtils::GenerateUniqueSkillID()
{
	return GetSkillIDGenerator().GenerateUniqueID();
}

UDSGameUtils::FSkillIDGenerator& UDSGameUtils::GetSkillIDGenerator()
{
	static FSkillIDGenerator Instance;
	return Instance;
}


