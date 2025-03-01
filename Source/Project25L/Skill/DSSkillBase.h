// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Default
#include "CoreMinimal.h"

//UE
#include "UObject/NoExportTypes.h"

//Game
#include "DSSkillSpec.h"
#include "System/DSEnums.h"
#include "DSSkillLifeCycle.h"
#include "DSSkillActorInfo.h"

//UHT
#include "DSSkillBase.generated.h"

struct FDSSkillSpec;
struct FDSSkillActorInfo;
class UDSSkillControlComponent;

/**
 * 
 */
 // ----------------------------------------------------------------------------------------------------------------
//
//	The important functions:
//	
//		CanActivateSkill()	- const function to see if skill is activatable. Callable by UI, Input
//
//		TryActivateSkill()	- Attempts to activate the skill. Calls CanActivateSkill(). Input events can call this directly.
//								- Also handles instancing-per-execution logic and replication/prediction calls.
//		
//		CallActivateSkill()	- Protected, non virtual function. Does some boilerplate 'pre activate' stuff, then calls ActivateSkill()
//
//		ActivateSkill()		- What the abilities *does*. This is what child classes want to override.
//	
//		CommitSkill()			- Commits reources/cooldowns etc. ActivateSkill() must call this!
//		
//		CancelSkill()			- Interrupts the skill (from an outside source).
//
//		EndSkill()			- The skill has ended. This is intended to be called by the skill to end itself.
//	
// ----------------------------------------------------------------------------------------------------------------
UCLASS()
class PROJECT25L_API UDSSkillBase : public UObject, public IDSSkillLifeCycle
{
	GENERATED_BODY()

	friend class UDSSkillControlComponent;

public:
	UDSSkillBase();
	UDSSkillBase(const FObjectInitializer& ObjectInitializer);

	bool IsSupportedForNetworking() const
	{
		/**
		 *	We can only replicate references to:
		 *		-CDOs and DataAssets (e.g., static, non-instanced skills)
		 *		-Instanced skills that are replicating (and will thus be created on clients).
		 *
		 *	Otherwise it is not supported, and it will be recreated on the client
		 */

		bool Supported = GetReplicationPolicy() != ESkillReplicationPolicy::ReplicateNo || GetOuter()->IsA(UPackage::StaticClass());

		return Supported;
	}

	// --------------------------------------
//	Accessors
// --------------------------------------
	/** How an skill replicates state/events to everyone on the network */
	ESkillReplicationPolicy GetReplicationPolicy() const
	{
		return ReplicationPolicy;
	}

	/** Returns how the skill is instanced when executed. This limits what an skill can do in its implementation. */
	ESkillInstancingPolicy GetInstancingPolicy() const
	{
		return InstancingPolicy;
	}
	
	/** Where does an skill execute on the network? Does a client "ask and predict", "ask and wait", "don't ask (just do it)" */
	ESkillNetExecutionPolicy GetNetExecutionPolicy() const
	{
		return NetExecutionPolicy;
	}
	
	/** Where should an skill execute on the network? Provides protection from clients attempting to execute restricted skills. */
	ESkillNetSecurityPolicy GetNetSecurityPolicy() const
	{
		return NetSecurityPolicy;
	}

	// --------------------------------------
	//	Interaction with Skill Control component
	// --------------------------------------

	/** Called by skill SkillControlComponent to inform this skill instance the remote instance was ended */
	virtual void SetRemoteInstanceHasEnded();

	/** Called to inform the skill that the AvatarActor has been replaced. If the skill is dependent on avatar state, it may want to end itself. */
	virtual void NotifyAvatarDestroyed();

	/** Called when the skill is Added to an SkillControlComponent */
	virtual void OnAddSkill(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec);

	/** Called when the skill is removed from an SkillControlComponent */
	virtual void OnRemoveSkill(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec) ;

	/** Called when the avatar actor is set/changes */
	virtual void OnAvatarSet(const FDSSkillActorInfo* ActorInfo, const FDSSkillSpec& Spec);

	/** Called to initialize after being created due to replication */
	virtual void PostNetInit();

	/** Called on a predictive skill when the server confirms its execution */
	virtual void ConfirmActivateSucceed();

	/** Takes in the skill spec and checks if we should allow replication on the skill spec, this will NOT stop replication of the skill UObject just the spec inside the USkillControlComponenet ActivatableSkills for this skill */
	virtual bool ShouldReplicateSkillSpec(const FDSSkillSpec& SkillSpec) const
	{
		return true;
	}

	// --------------------------------------
	//	ActorInfo
	// --------------------------------------
	const FDSSkillActorInfo* GetCurrentActorInfo() const;

	/** Modifies actor info, only safe on instanced abilities */
	virtual void SetCurrentActorInfo(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const;

	bool IsActive() const;

	bool IsInstantiated() const;

	// 생명주기 함수와, 내부 구현 함수의 역할 잘 구분하기
	/*
	인터페이스 함수:
	GetSkillOwner, GetSkillAvatar, GetSkillControlComponent, OnSkillInitialized, OnSkillActivated, OnSkillDeactivated
	→ 역할: 스킬의 생명주기와 관련된 외부와의 상호작용(액터, 컨트롤 컴포넌트, 초기화/활성화/비활성화 이벤트 처리)을 강제하는 계약
	
	기본 클래스(내부 구현) 함수:
	CanActivateSkill, TryActivateSkill, CallActivateSkill, ActivateSkill, CommitSkill, CancelSkill, EndSkill
	→ 역할: 스킬의 활성화 및 실행 흐름에 대한 기본 로직을 제공하며, 자식 클래스에서 구체적인 기능을 확장하도록 함
	*/

	// --------------------------------------
	//	The important functions
	// --------------------------------------
	// 스킬 활성화 가능 여부(자원, 쿨타임, 상태 조건 등)를 확인하는 함수
	virtual bool CanActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	
	// 스킬 활성화 전의 사전 처리(예: 상태 초기화, 로깅, 전처리 작업 등)를 수행한 후 실제 ActivateSkill()를 호출하는 함수
	virtual void CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	
	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	
	// 능력 활성화와 동시에 자원 소모, 쿨타임 시작, 기타 상태 변화를 커밋(확정)하는 함수
	virtual bool CommitSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	
	// 외부 요인에 의해 스킬 실행을 중단하고 정리하는 함수
	virtual void CancelSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool bReplicateCancelSkill);
	
	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled);

	// 현재 Skill을 시전 전까지 얼만큼의 시간이 남았는지 확인하는 함수
	virtual void GetCooldownTimeRemainingAndDuration(FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const;

protected:
	// --------------------------------------
	//	Cooldown
	// --------------------------------------
	/** 재사용 대기 시간을 확인하는 함수. 다시 사용할 수 있으면 true, 그렇지 않으면 false return*/
	virtual bool CheckCooldown(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const;
	virtual void ApplyCooldown(const FDSSkillActorInfo* ActorInfo);

	// --------------------------------------
	//	ISkillLifeCycle
	// --------------------------------------	
	FName SkillName;

	/** 스킬을 소유한 Actor 반환 */
	virtual AActor* GetSkillOwner() const override;

	/** 스킬을 실행하는 Actor 반환 */
	virtual AActor* GetSkillAvatar() const override;

	/** 스킬을 관리하는 SkillControlComponent 반환 */
	virtual UDSSkillControlComponent* GetSkillControlComponent() const override;

	/** 스킬 초기화 시 호출 */
	virtual void OnSkillInitialized(UDSSkillBase& Skill) override;

	/** 스킬 활성화 시 호출 */
	virtual void OnSkillActivated(UDSSkillBase& Skill) override;

	/** 스킬 비활성화 시 호출 */
	virtual void OnSkillDeactivated(UDSSkillBase& Skill) override;

private:
	// --------------------------------------
	//	CanActivate
	// --------------------------------------
		/** Returns true if an an skill should be activated */
	virtual bool ShouldActivateSkill(ENetRole Role) const;
	/** 이 스킬을 사용하는 동안 다른 스킬들을 막을 것인지*/
	virtual void SetShouldBlockOtherSkills(bool bShouldBlockSkills);

	// --------------------------------------
	//	CallActivateSkill
	// --------------------------------------
	virtual void PreActivate(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	// --------------------------------------
	//	CancelSkill
	// --------------------------------------
	/** Returns true if this Skill can be canceled */
	virtual bool CanBeCanceled() const;

	/** Sets whether the Skill should ignore cancel requests. Only valid on instanced skills */
	UFUNCTION(BlueprintCallable, Category=Skill)
	virtual void SetCanBeCanceled(bool bCanBeCanceled);

	// --------------------------------------
	//	CommitSkill
	// --------------------------------------
	/** The last chance to fail before committing, this will usually be the same as CanActivateSkill. Some abilities may need to do extra checks here if they are consuming extra stuff in CommitExecute */
	virtual bool CommitCheck(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo); 

	// --------------------------------------
	//	EndSkill
	// --------------------------------------
	virtual bool IsEndSkillValid(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const;

	// --------------------------------------
	//	Cost
	// --------------------------------------
	/** Checks cost. returns true if we can pay for the skill. False if not */
	virtual bool CheckCost(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo) const;
	virtual void ApplyCost(const FDSSkillActorInfo* ActorInfo);

public:
	/** if true, and trying to activate an already active instanced skill, end it and re-trigger it. */
	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	uint8 bRetriggerInstancedSkill :1;


protected:	
	// -------------------------------------
	//	Protected properties
	// -------------------------------------
	/** How an skill replicates state/events to everyone on the network. Replication is not required for NetExecutionPolicy. */
	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	ESkillReplicationPolicy ReplicationPolicy;

	/** How the skill is instanced when executed. This limits what an skill can do in its implementation. */
	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	ESkillInstancingPolicy	InstancingPolicy;

		/** How does an skill execute on the network. Does a client "ask and predict", "ask and wait", "don't ask (just do it)". */
	UPROPERTY(EditDefaultsOnly, Category=Advanced)
	ESkillNetExecutionPolicy NetExecutionPolicy = ESkillNetExecutionPolicy::ServerInitiated;

	UPROPERTY(EditDefaultsOnly, Category = Advanced)
	ESkillNetSecurityPolicy NetSecurityPolicy;

	mutable const FDSSkillActorInfo* CurrentActorInfo;

	mutable FDSSkillSpecHandle CurrentSpecHandle;


/////////////////////////////////////////////////////// SkillSpec ///////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, Category ="Cooltime")
	float MaxCooltime = 100.f;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cooltime")
	uint8 bSkillHasCooltime : 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cooltime")
	uint8 bCooldown: 1;

	float StartWorldTime;
	/** True if the skill is currently cancelable, if not will only be canceled by hard EndSkill calls */
	UPROPERTY()
	uint8 bIsCancelable : 1;

	/** True if the skill is currently active. For instance per owner skills */
	UPROPERTY()
	uint8 bIsActive : 1;

	/** True if the skill block flags are currently enabled */
	UPROPERTY()
	uint8 bIsBlockingOtherSkills : 1;

	UPROPERTY()
	uint8 RemoteInstanceEnded : 1;
	UPROPERTY()
	uint8 bIsSkillEnding : 1;

};

