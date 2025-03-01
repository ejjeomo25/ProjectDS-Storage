
#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "Delegates/Delegate.h"
#include "UObject/ObjectMacros.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSEventSystems.generated.h"

#define DSEVENT_DELEGATE_BIND(EventName, Object, FuncPtr)	\
{															\
    EventName.AddUObject(Object, FuncPtr);					\
}															\

#define DSEVENT_DELEGATE_INVOKE(EventName, ...)				\
{															\
	if (EventName.IsBound())								\
	{														\
		EventName.Broadcast(__VA_ARGS__);					\
	}														\
}
													
#define DSEVENT_DELEGATE_REMOVE(EventName, Object)			\
{															\
	EventName.RemoveAll(Object);							\
}															\


class UMaterialInterface;

UCLASS()
class PROJECT25L_API UDSEventSystems : public UObject
{
	GENERATED_BODY()
	
public:
	static UDSEventSystems* Get(UObject *Object);
};

struct DSGameUI
{

};

struct DSGameEvent
{
	//임시 예시
	DECLARE_MULTICAST_DELEGATE(DS_DELEGATE);
	DS_DELEGATE OnDs_delegate;
	DS_DELEGATE OnAllAssetsLoaded;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGimmickStateChanged, bool);
	FOnGimmickStateChanged OnGimmickStateChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDungeonStageChanged, EDungeonType);
	FOnDungeonStageChanged OnUnlockStage;
	FOnDungeonStageChanged OnDestroyStage;

	DECLARE_MULTICAST_DELEGATE_OneParam(DS_ONE_FLOAT_DELEGATE, float);
	DS_ONE_FLOAT_DELEGATE OnHPChanged;

};

struct DSSkillInputEvent
{
	DSSkillInputEvent()
	{
		// OnSkillPressedEvents.Add(ESkillType::MouseLSkill, FOnSkillPressed());
		// OnSkillPressedEvents.Add(ESkillType::MouseRSkill, FOnSkillPressed());
		// OnSkillPressedEvents.Add(ESkillType::QSkill, FOnSkillPressed());
		// OnSkillPressedEvents.Add(ESkillType::XSkill, FOnSkillPressed());
		// OnSkillPressedEvents.Add(ESkillType::ESKill, FOnSkillPressed());
		// OnSkillPressedEvents.Add(ESkillType::RSkill, FOnSkillPressed());
	}
	// Input
	DECLARE_MULTICAST_DELEGATE(FOnWeaponToggle);
	FOnWeaponToggle OnWeaponToggle;

	DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionStarted);
	FOnWeaponPrimaryActionStarted OnWeaponPrimaryActionStarted;

	DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionOngoing);
	FOnWeaponPrimaryActionOngoing OnWeaponPrimaryActionOngoing;

	DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionReleased);
	FOnWeaponPrimaryActionReleased OnWeaponPrimaryActionReleased;

	DECLARE_MULTICAST_DELEGATE(FOnWeaponSecondaryAction);
	FOnWeaponSecondaryAction OnWeaponSecondaryAction;

	// Skill
	// DECLARE_MULTICAST_DELEGATE(FOnSkillPressed);
	// TMap< ESkillType, FOnSkillPressed> OnSkillPressedEvents;
};

// 전역 객체 선언
extern DSGameEvent GameEvent;
extern DSGameUI GameUI;
extern DSSkillInputEvent SkillInputEvent;