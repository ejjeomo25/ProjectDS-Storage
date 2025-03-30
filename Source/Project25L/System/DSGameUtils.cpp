// Default
#include "System/DSGameUtils.h"

// UE
#include "GameFramework/PlayerController.h"

// Game
#include "System/DSGameInstance.h"
#include "Character/DSCharacter.h"


TArray<float> UDSGameUtils::SpreadOffsets;

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

bool UDSGameUtils::IsWithinCharacterFOV(const ACharacter* Character, const AActor* Target, float AnchorAngle)
{
	//만약에 캐릭터라면 캐릭터가 바라보는 forward vector 가져온다.
	const FVector& CharacterForwardVec = Character->GetActorForwardVector();
	FVector CharacterLoc = Character->GetActorLocation(); //Z값을 고려하지 않는다.
	FVector TargetLoc = Target->GetActorLocation();

	// Z를 동일하게 맞춰준다.
	// 이유: Z값을 고려할 경우, 내적에 해당하는 방향 값이 달라진다.
	CharacterLoc.Z = 0;
	TargetLoc.Z = 0;

	// 캐릭터를 기준으로 방향을 알아냄
	FVector DirectionToTarget = TargetLoc - CharacterLoc;
	DirectionToTarget.Normalize(); // Character가 바라보는 forward vector 값이 노멀된 값이기 때문에 노멀라이즈 해준다.

	// 두 벡터간의 내적을 통해서 cos = radian 값 구하기
	float Dot = FVector::DotProduct(CharacterForwardVec, DirectionToTarget);

	// 실제 각도의 radian 값
	float CosThreshold = FMath::Cos(FMath::DegreesToRadians(AnchorAngle));

	// 두 값을 비교
	return Dot >= CosThreshold;
}

int UDSGameUtils::GetSpreadOffsetIdx()
{
	int32 Idx = FMath::RandRange(0, SpreadOffsets.Num());
	return Idx;
}

float UDSGameUtils::GetSpreadOffset(int Idx)
{
	return SpreadOffsets[Idx];
}

void UDSGameUtils::GenerateUniqueSpreadOffset(float SpreadCoef)
{
	TSet<int32> UniqueOffsets;

	// float 값은 정밀도 계산이 어렵기 때문에, int32를 사용해서 정밀도를 높여준다.
	while (SpreadOffsets.Num() < 100)
	{
		float Raw = FMath::FRandRange(-SpreadCoef, SpreadCoef);
		int32 Key = FMath::RoundToInt(Raw * 10000.0f); // 정밀도 0.0001f

		// Set을 사용해서, 중복 검사를 O(1) 수준으로 만들 수 있다.
		if (UniqueOffsets.Contains(Key) == false)
		{
			UniqueOffsets.Add(Key);
			SpreadOffsets.Add(Raw);
		}
	}
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

void UDSGameUtils::LoadSpreadOffset(FString Path, float SpreadCoef)
{
	bool bIsSuccess = false;

	FString Result = ReadFromFile(Path, bIsSuccess);
	
	//만약 탄퍼짐이 이미 구현되어 있다면, 데이터를 로드 후 랜덤으로 Idx 값을 뽑는다.
	//탄 퍼짐이 구현되어있지 않다면, 가져온 뒤 데이터를 저장한다.
	if (bIsSuccess)
	{
		//파일이 존재함
		//'\n'을 중심으로 자름
		TArray<FString> Lines;
		Result.ParseIntoArrayLines(Lines);

		for (const FString& Line : Lines)
		{
			if (!Line.IsEmpty())
			{
				float Value = FCString::Atof(*Line);
				SpreadOffsets.Add(Value);
			}
		}

	}
	else
	{
		//파일이 존재하지 않아서 Array 생성 후 파일로 저장
		GenerateUniqueSpreadOffset(SpreadCoef);

		FString WritingString = "";
		for (int OffsetIdx = 0; OffsetIdx < SpreadOffsets.Num(); OffsetIdx++)
		{
			float Offset = SpreadOffsets[OffsetIdx];

			WritingString += FString::Printf(TEXT("%f\n"), Offset);
		}

		//저장
		WriteToFile(Path, WritingString, bIsSuccess);
	}
}

FString UDSGameUtils::ReadFromFile(FString FilePath, bool& bOutSuccess)
{
	FPlatformFileManager &PlatformFileManager = FPlatformFileManager::Get();

	IPlatformFile& PlatformFile = PlatformFileManager.GetPlatformFile();

	FString RetString = "";
	if (!PlatformFile.FileExists(*FilePath))
	{
		bOutSuccess = false;
		return RetString;
	}

	if (FFileHelper::LoadFileToString(RetString, *FilePath) == false)
	{
		bOutSuccess = false;
		return RetString;
	}
	bOutSuccess = true;
	return RetString;
}

void UDSGameUtils::WriteToFile(FString FilePath, FString String, bool& bOutSuccess)
{
	if (FFileHelper::SaveStringToFile(String, *FilePath) == false)
	{
		bOutSuccess = false;
		return;
	}

	bOutSuccess = true;
}

UDSGameUtils::FSkillIDGenerator& UDSGameUtils::GetSkillIDGenerator()
{
	static FSkillIDGenerator Instance;
	return Instance;
}