
#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Animation/AnimMetaData.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSAttackAnimMetaData.generated.h"

UCLASS()
class PROJECT25L_API UDSAttackAnimMetaData : public UAnimMetaData
{
	GENERATED_BODY()
	

public:
	ESkillType GetSkillType() const { return SkillType; }
	int32 GetComboCount() const { return MaxSequenceCount; }

protected:
	UPROPERTY(EditAnywhere, Category = "DSSettings | Animation")
	int32 MaxSequenceCount;

	UPROPERTY(EditDefaultsOnly, Category = "DSSettings | Skill")
	ESkillType SkillType;

};
