// Default
#include "UI/Game/Player/DS_HPBar.h"

// UE
#include "Components/ProgressBar.h"

// Game
#include "System/DSGameDataSubsystem.h"

#include "DSLogChannels.h"

UDS_HPBar::UDS_HPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDS_HPBar::SetHP(float CurrentHP, float MaxHP)
{
	if (IsValid(ProgressBar_HP))
	{
		float Percentage = 0.0f;
		if (false == FMath::IsNearlyZero(MaxHP))
		{
			Percentage = CurrentHP / MaxHP; //이미 HP는 100퍼센트로 계산되어서 전달되었음.
		}
		ProgressBar_HP->SetPercent(Percentage);
	}
}
