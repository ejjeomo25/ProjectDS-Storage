// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Base/DSTestSkill.h"

UDSTestSkill::UDSTestSkill()
:Super()
{
	bSkillHasCooltime = true;
	bSkillHasDuration = true;
}

UDSTestSkill::UDSTestSkill(FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bSkillHasCooltime = true;
	bSkillHasDuration = true;
}



// 
UDSTestSkill_1::UDSTestSkill_1()
{
	bSkillHasCooltime = false;

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

UDSTestSkill_1::UDSTestSkill_1(FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSkillHasCooltime = false;

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

UDSTestSkill_2::UDSTestSkill_2()
{
	bSkillHasCooltime = false;
}

UDSTestSkill_2::UDSTestSkill_2(FObjectInitializer& ObjectInitializer)
{
	bSkillHasCooltime = false;
}
