//Default
#include "GameData/GameplayTag/DSGameplayTags.h"

// UE
#include "GameplayTagsManager.h"

FDSTags FDSTags::GameplayTags;

void FDSTags::InitializeNativeGameplayTags()
{
	//  GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Move"), FString("이동 버튼 Input Tag"));

	//********************************************************
	// Enhanced Input
	// Move
	GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Move"));
	GameplayTags.InputTag_Look_Mouse = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Look.Mouse"));
	GameplayTags.InputTag_Jump = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Jump"));
	GameplayTags.InputTag_Parkour = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Parkour"));
	GameplayTags.InputTag_FastRun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.FastRun"));
	GameplayTags.InputTag_Sit = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Sit"));

	// System
	GameplayTags.InputTag_Pause = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Pause"));
	GameplayTags.InputTag_Interaction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Interaction"));

	// Skill
	GameplayTags.InputTag_Skill_PrimarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.PrimarySkill"));
	GameplayTags.InputTag_Skill_SecondarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.SecondarySkill"));
	GameplayTags.InputTag_Skill_Skill1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Skill1"));
	GameplayTags.InputTag_Skill_Skill2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Skill2"));
	GameplayTags.InputTag_Skill_UltimateSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.UltimateSkill"));
	GameplayTags.InputTag_Skill_FarmingSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.FarmingSkill"));


	// Item
	GameplayTags.InputTag_Item_QuickSlot1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot1"));
	GameplayTags.InputTag_Item_QuickSlot2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot2"));
	GameplayTags.InputTag_Item_QuickSlot3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot3"));
	GameplayTags.InputTag_Item_Vehicle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.Vehicle"));

	// Weapon
	GameplayTags.InputTag_Weapon_Toggle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Toggle"));
	GameplayTags.InputTag_Weapon_Equipment_toggle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Equipment_toggle"));

	// UI
	GameplayTags.InputTag_UI_Inventory = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.UI.Inventory"));
	GameplayTags.InputTag_UI_Status = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.UI.Status"));

	// FilghtSkill
	GameplayTags.InputTag_Skill_Flight_Begin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Begin"));
	GameplayTags.InputTag_Skill_Flight_Up = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Up"));
	GameplayTags.InputTag_Skill_Flight_Down = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Down"));
	GameplayTags.InputTag_Skill_Flight_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Dodge"));
	GameplayTags.InputTag_Skill_Flight_Boost = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Flight.Boost"));

	//********************************************************
	// UI Layer
	GameplayTags.UI_Layer_GameMenu = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameMenu"));
	GameplayTags.UI_Layer_Modal = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Modal"));
	GameplayTags.UI_Layer_Game = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Game"));
	GameplayTags.UI_Layer_Game_Main = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.Game.Main"));

	GameplayTags.UI_Layer_GameMenu_Inventory = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameMenu.Inventory"));
	

	//********************************************************
	// Skill
	GameplayTags.Skill_PrimarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.PrimarySkill"));
	GameplayTags.Skill_SecondarySkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.SecondarySkill"));
	GameplayTags.Skill_Skill1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.Skill1"));
	GameplayTags.Skill_Skill2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.Skill2"));
	GameplayTags.Skill_FarmingSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.FarmingSkill"));
	GameplayTags.Skill_UltimateSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.UltimateSkill"));
	GameplayTags.Skill_TestSkill = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.TestSkill"));
	GameplayTags.Skill_Skill1_Installation = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.Skill1.Installation"));
	GameplayTags.Skill_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Skill.NoneSkill"));
	
}
