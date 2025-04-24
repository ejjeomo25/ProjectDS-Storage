#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameplayTagContainer.h"


struct FDSTags
{
public:
	static const FDSTags& GetDSTags() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	//********************************************************
	// Enhanced Input

	// Move
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Parkour;
	FGameplayTag InputTag_FastRun;
	FGameplayTag InputTag_Sit;

	// System
	FGameplayTag InputTag_Pause;
	FGameplayTag InputTag_Interaction;

	// Skill
	FGameplayTag InputTag_Skill_PrimarySkill;
	FGameplayTag InputTag_Skill_SecondarySkill;
	FGameplayTag InputTag_Skill_Skill1;
	FGameplayTag InputTag_Skill_Skill2;
	FGameplayTag InputTag_Skill_UltimateSkill;
	FGameplayTag InputTag_Skill_FarmingSkill;

	// Item
	FGameplayTag InputTag_Item_QuickSlot1;
	FGameplayTag InputTag_Item_QuickSlot2;
	FGameplayTag InputTag_Item_QuickSlot3;
	FGameplayTag InputTag_Item_Vehicle;

	// Weapon
	FGameplayTag InputTag_Weapon_Toggle;
	FGameplayTag InputTag_Weapon_Equipment_toggle;

	// UI
	FGameplayTag InputTag_UI_Inventory;
	FGameplayTag InputTag_UI_Status;

	// FlightSkill
	FGameplayTag InputTag_Skill_Flight_Begin;
	FGameplayTag InputTag_Skill_Flight_Up;
	FGameplayTag InputTag_Skill_Flight_Down;
	FGameplayTag InputTag_Skill_Flight_Dodge;
	FGameplayTag InputTag_Skill_Flight_Boost;

	//********************************************************
	// Layer UI
	FGameplayTag UI_Layer_GameMenu;
	FGameplayTag UI_Layer_Modal;

	FGameplayTag UI_Layer_GameMenu_Inventory;
	FGameplayTag UI_Layer_Game;
	FGameplayTag UI_Layer_Game_Main;


	//********************************************************
	// Skill
	FGameplayTag Skill_None;
	FGameplayTag Skill_PrimarySkill;
	FGameplayTag Skill_SecondarySkill;
	FGameplayTag Skill_Skill1;
	FGameplayTag Skill_Skill1_Installation;
	FGameplayTag Skill_Skill2;
	FGameplayTag Skill_FarmingSkill;
	FGameplayTag Skill_UltimateSkill;
	FGameplayTag Skill_TestSkill;

private:
	static FDSTags GameplayTags;
};