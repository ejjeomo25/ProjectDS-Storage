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
	GameplayTags.InputTag_Skill_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.1"));
	GameplayTags.InputTag_Skill_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.2"));
	GameplayTags.InputTag_Skill_Ultimate = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Ultimate"));
	GameplayTags.InputTag_Skill_Farming = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Skill.Farming"));

	// Item
	GameplayTags.InputTag_Item_QuickSlot1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot1"));
	GameplayTags.InputTag_Item_QuickSlot2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot2"));
	GameplayTags.InputTag_Item_QuickSlot3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.QuickSlot3"));
	GameplayTags.InputTag_Item_Vehicle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Item.Vehicle"));

	// Weapon
	GameplayTags.InputTag_Weapon_Toggle = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.Toggle"));
	GameplayTags.InputTag_Weapon_PrimaryAction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.PrimaryAction"));
	GameplayTags.InputTag_Weapon_SecondaryAction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Weapon.SecondaryAction"));
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

	GameplayTags.UI_Layer_GameMenu_Inventory = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("UI.Layer.GameMenu.Inventory"));

}
