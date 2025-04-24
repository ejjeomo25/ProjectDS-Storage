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
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct  FDSSkillAttribute : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute() :
		MaxCooltime(0.0f)
		, AutoAimAngle(0.0f)
		, AttackRange(0.f)
		, InputThresholdOffset(0.f)
		, SkillDuration(0.f)
		, Effects()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float MaxCooltime;

	/*오토 타겟팅용 각도를 의미*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float AutoAimAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float InputThresholdOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	float SkillDuration;

	/*스킬 발동시 보여줄 이펙트*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DSSettings | Skill")
	TMap<ESkillActivationStatus, TSoftObjectPtr<UNiagaraSystem>> Effects;
};

USTRUCT(BlueprintType)
struct  FDSSkillAttribute_Girl : public FDSSkillAttribute
{
	GENERATED_USTRUCT_BODY()

public:
	FDSSkillAttribute_Girl() :
		AttackRadius(0.0f)
		, GravityImpulse(0.0f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float AttackRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float GravityImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float SkillDelay;
};
