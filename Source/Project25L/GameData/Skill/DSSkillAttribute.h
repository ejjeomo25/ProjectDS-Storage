#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

// Game
#include "GameData/DSEnums.h"

// UHT
#include "DSSkillAttribute.generated.h"

/**
 *  스킬의 수치 데이터를 관리하는 DataTable
 */

 USTRUCT(BlueprintType)
struct  FDSSkillAttribute : public FTableRowBase
{
GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute() :
		MaxCooltime(0.0f)
		, AutoAimAngle(0.0f)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float MaxCooltime;

	/*오토 타겟팅용 각도를 의미*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float AutoAimAngle;

};
