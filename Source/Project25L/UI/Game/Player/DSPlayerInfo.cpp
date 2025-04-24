// Default
#include "UI/Game/Player/DSPlayerInfo.h"

// UE
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Game
#include "System/DSGameDataSubsystem.h"
#include "Player/DSPlayerController.h"

UDSPlayerInfo::UDSPlayerInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDSPlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();

	//플레이어 이미지로 로드한다.
	ADSPlayerController* PlayerController = GetOwningPlayer<ADSPlayerController>();
	
	if (IsValid(PlayerController))
	{
		ECharacterType CharacterType = PlayerController->GetCharacterType();

		UDSGameDataSubsystem *DataSubsystem = UDSGameDataSubsystem::Get(PlayerController);

		check(DataSubsystem);

	}
}

void UDSPlayerInfo::SetText(float CurrentHP, float MaxHP)
{
	if (IsValid(Text_HP))
	{
		//정수형으로 출력한다.
		FString Text = FString::Printf(TEXT("%.0f"), CurrentHP);

		Text_HP->SetText(FText::FromString(Text));
	}

	if (IsValid(Text_MaxHP))
	{
		FString Text = FString::Printf(TEXT("%.0f"), MaxHP);
		Text_HP->SetText(FText::FromString(Text));
	}
}
