// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Defualt 
#include "CoreMinimal.h"

// Game
#include "Character/DSCharacter.h"
#include "System/DSEnums.h"
#include "Skill/DSSkillSpec.h"

// UHT
#include "DSCharacter_Girl.generated.h"


// Input
DECLARE_MULTICAST_DELEGATE(FOnSkillPressed);
// 
// 
// DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionStarted);
// 
// DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionOngoing);
// 
// DECLARE_MULTICAST_DELEGATE(FOnWeaponPrimaryActionReleased);
// 
// DECLARE_MULTICAST_DELEGATE(FOnWeaponSecondaryAction);
// FOnWeaponSecondaryAction OnWeaponSecondaryAction;
// 
// // Skill
// DECLARE_MULTICAST_DELEGATE(FOnSkillPressed);
// TMap< ESkillType, FOnSkillPressed> OnSkillPressedEvents;
// 
// FOnWeaponToggle OnWeaponToggle;
// FOnWeaponPrimaryActionStarted OnWeaponPrimaryActionStarted;
// FOnWeaponPrimaryActionOngoing OnWeaponPrimaryActionOngoing;
// FOnWeaponPrimaryActionReleased OnWeaponPrimaryActionReleased;



/**
 * 
 */
UCLASS()
class PROJECT25L_API ADSCharacter_Girl : public ADSCharacter
{
	GENERATED_BODY()

public:
	ADSCharacter_Girl(const FObjectInitializer& ObjectInitializer);

	TMap< ESkillType, FOnSkillPressed> OnSkillPressedEvents;
protected:
	virtual void InitSkillActorInfo() override;

	// 이 아래로는 ADSCharacter 로 옮겨야 한다.
	TMap<ESkillType, FDSSkillSpecHandle> SkillSpecHandles;

	// 캐릭터가 리셋될 때 (리스폰 전, 죽은 후)
	virtual void Reset();

	// 여기는 함수 이름 바꿔서 옮겨야 한다.
	virtual void ActivateTestSkill1();

	virtual void ActivateTestSkill2();
	virtual void ActivateTestSkill3();


	UFUNCTION(Server, Unreliable)
	void ServerRPC_ActivateTestSkill1();

	UFUNCTION(Client, Unreliable)
	void ClientRPC_PlaySkillAnimation(ADSCharacter_Girl* Player);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_PlaySkillAnimation();
};
