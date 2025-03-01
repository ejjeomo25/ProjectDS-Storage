// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/DSTestSkill.h"

UDSTestSkill::UDSTestSkill()
:Super()
{
	bSkillHasCooltime = true;
	SkillName = FName("TestSkill");
}

UDSTestSkill::UDSTestSkill(FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bSkillHasCooltime = true;
	SkillName = FName("TestSkill");
}



// 
UDSTestSkill_1::UDSTestSkill_1()
{
	bSkillHasCooltime = false;
	SkillName = FName("TestSkill1");

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}

UDSTestSkill_1::UDSTestSkill_1(FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSkillHasCooltime = false;
	SkillName = FName("TestSkill1");

	InstancingPolicy = ESkillInstancingPolicy::NonInstanced;
}




UDSTestSkill_2::UDSTestSkill_2()
{
	bSkillHasCooltime = false;
	SkillName = FName("TestSkill2");
}

UDSTestSkill_2::UDSTestSkill_2(FObjectInitializer& ObjectInitializer)
{
	bSkillHasCooltime = false;
	SkillName = FName("TestSkill2");
}
