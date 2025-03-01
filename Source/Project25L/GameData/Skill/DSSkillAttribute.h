// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "Engine/DataTable.h"

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
		MaxCooltime(0.0f) { }
	// FDSSkillAttribute(float NewMaxCooltime) :
	// 	MaxCooltime(NewMaxCooltime) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SkillAttribute)
	float MaxCooltime;
};
