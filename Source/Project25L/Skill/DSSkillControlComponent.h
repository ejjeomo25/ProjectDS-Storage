#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "Components/PawnComponent.h"

//Game
#include "DSSkillSpec.h"
#include "DSSkillActorInfo.h"
#include "Player/DSCheatManager.h"

//UHT
#include "DSSkillControlComponent.generated.h"

UCLASS()
class PROJECT25L_API UDSSkillControlComponent : public UPawnComponent
{
	GENERATED_BODY()
	friend class UDSCheatManager;

public:
	UDSSkillControlComponent(const FObjectInitializer& ObjectInitializer);
	
	/** called when the Skill Control Component SkillActorInfo has a PlayerController set. */
	virtual void OnPlayerControllerSet() { }

	// --------------------------------------
	//	The important functions
	// --------------------------------------

	FDSSkillSpecHandle AddSkill(const FDSSkillSpec& Spec);

	virtual bool TryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);

	/** Will be called from AddSkill or from OnRep. Initializes events (inputs) with the add skill */
	virtual void OnAddSkill(FDSSkillSpec& SkillSpec);

	/** Will be called from RemoveSkill or from OnRep. Unbinds inputs with the given skill */
	virtual void OnRemoveSkill(FDSSkillSpec& SkillSpec);
	/**
 * Removes the specified skill.
 * This will be ignored if the actor is not authoritative.
 *
 * @param Handle Skill Spec Handle of the skill we want to remove
 */
	void ClearSkill(const FDSSkillSpecHandle& Handle);

	// ----------------------------------------------------------------------------------------------------------------
	// Skill Cancelling/Interrupts
	// ----------------------------------------------------------------------------------------------------------------

	/** Cancels the specified skill CDO. */
	void CancelSkill(UDSSkillBase* Skill);

	/** Cancels the skill indicated by passed in spec handle. If handle is not found among reactivated abilities nothing happens. */
	void CancelSkillHandle(const FDSSkillSpecHandle& SkillHandle);

	/** Cancels all abilities regardless of tags. Will not cancel the ignore instance */
	void CancelAllSkills(UDSSkillBase* Ignore = nullptr);


	// ----------------------------------------------------------------------------------------------------------------
	// Call notify callbacks above
	// ----------------------------------------------------------------------------------------------------------------

	/** Called from the skill to let the component know it is commit */
	virtual void NotifySkillCommit(UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is Activated */
	virtual void NotifySkillActivated(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is Failed */
	virtual void NotifySkillFailed(const FDSSkillSpecHandle Handle, UDSSkillBase* Skill);
	/** Called from the skill to let the component know it is ended */
	virtual void NotifySkillEnded(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCancelled);

	virtual void ReplicateEndOrCancelSkill(FDSSkillSpecHandle Handle, UDSSkillBase* Skill, bool bWasCanceled);


	// ----------------------------------------------------------------------------------------------------------------
	//  FindSkillSpec
	// ----------------------------------------------------------------------------------------------------------------

	/** Returns an skill spec corresponding to given skill class. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromClass(TSubclassOf<UDSSkillBase> InSkillClass) const;

	/** Returns an skill spec from a handle. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromInputID(int32 InputID) const;

	/** Returns an skill spec from a handle. If modifying call MarkSkillSpecDirty */
	FDSSkillSpec* FindSkillSpecFromHandle(FDSSkillSpecHandle Handle) const;
	

	// ----------------------------------------------------------------------------------------------------------------
	//	Actor interaction
	// ----------------------------------------------------------------------------------------------------------------	
	virtual void InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);

	void RefreshSkillActorInfo();

	virtual void ClearActorInfo();

	UFUNCTION()
	virtual void ClearAllSkills();

	void SetOwnerActor(AActor* NewOwnerActor);
	AActor* GetOwnerActor() const { return SkillOwnerActor; }

	void SetAvatarActor_Direct(AActor* NewAvatarActor);
	AActor* GetAvatarActor_Direct() const { return SkillAvatarActor; }

	void SetAvatarActor(AActor* InAvatarActor);
	AActor* GetAvatarActor() const;

	UFUNCTION()
	void OnAvatarActorDestroyed(AActor* InActor);

	UFUNCTION()
	void OnOwnerActorDestroyed(AActor* InActor);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

protected:
	/** Cancel a specific skill spec */
	virtual void CancelSkillSpec(FDSSkillSpec& Spec, UDSSkillBase* Ignore);

	UFUNCTION(Client, reliable)
	void ClientEndSkill(FDSSkillSpecHandle SkillToEnd); 

	UFUNCTION(Server, reliable, WithValidation)
	void ServerCancelSkill(FDSSkillSpecHandle SkillToCancel);

	UFUNCTION(Client, reliable)
	void ClientCancelSkill(FDSSkillSpecHandle SkillToCancel);

	void CallServerTryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool InputPressed); 
	void CallServerEndSkill(FDSSkillSpecHandle SkillToEnd);

	// ----------------------------------------------------------------------------------------------------------------
	// Functions meant to be called from skill and subclasses, but not meant for general use
	// ----------------------------------------------------------------------------------------------------------------

	/** Returns the list of all activatable Skills. Read-only. */
	const TArray<FDSSkillSpec>& GetActivatableAbilities() const
	{
		return ActivatableSkills.Items;
	}

	/** Returns the list of all activatable Skills. */
	TArray<FDSSkillSpec>& GetActivatableAbilities()
	{
		return ActivatableSkills.Items;
	}

	UFUNCTION(Server, reliable, WithValidation)
	void	ServerTryActivateSkill(FDSSkillSpecHandle SkillToActivate, bool InputPressed);

	UFUNCTION(Client, reliable)
	void	ClientTryActivateSkill(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION(Client, Reliable)
	void	ClientActivateSkillSucceed(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION(Server, reliable, WithValidation)
	void	ServerEndSkill(FDSSkillSpecHandle SkillToEnd);

	UFUNCTION(Client, Reliable)
	void	ClientActivateSkillFailed(FDSSkillSpecHandle SkillToActivate);

	UFUNCTION()
	void OnRep_ActivatableSkills();

	/** Add a gameplay skill associated to this component */
	void AddReplicatedInstancedSkill(UDSSkillBase* Skill);

	/** Remove a gameplay skill associated to this component */
	void RemoveReplicatedInstancedSkill(UDSSkillBase* Skill);

	/** Called by ServerEndSkill and ClientEndSkill; avoids code duplication. */
	void	RemoteEndOrCancelSkill(FDSSkillSpecHandle SkillToEnd,bool bWasCanceled);

	/** Implementation of ServerTryActivateSkill */
	virtual void InternalServerTryActivateSkill(FDSSkillSpecHandle AbilityToActivate, bool InputPressed);


private:
	virtual UDSSkillBase* CreateNewInstanceOfSkill(FDSSkillSpec& Spec, const UDSSkillBase* Skill);

	/** Call to mark that an skill spec has been modified */
	void MarkSkillSpecDirty(FDSSkillSpec& Spec, bool WasAddOrRemove = false);

	/** Returns true if this component's actor has authority */
	virtual bool IsOwnerActorAuthoritative() const;
	
	bool InternalTryActivateSkill(FDSSkillSpecHandle Handle, UDSSkillBase** OutInstancedAbility = nullptr);

	UFUNCTION()
	void OnRep_SkillOwnerActor();
	UFUNCTION()
	void OnRep_SkillAvatarActor();

public:
	FName AffectedAnimInstanceTag;
	/** Cached off data about the owning actor that abilities will need to frequently access (movement component, mesh component, anim instance, etc) */
	TSharedPtr<FDSSkillActorInfo>	SkillActorInfo;
	/** Checks if the Skill Control Component is currently blocking InputID. Returns true if InputID is blocked, false otherwise.  */
	bool IsSkillInputBlocked(int32 InputID) const;
	const TArray<uint8>& GetBlockedSkillBindings() const;



protected:
	/** A pending activation that cannot be activated yet, will be rechecked at a later point */
	struct FPendingSkillInfo
	{
		bool operator==(const FPendingSkillInfo& Other) const
		{
			// Don't compare event data, not valid to have multiple activations in flight with same key and handle but different event data
			return Handle == Other.Handle;
		}

		/** Properties of the skill that needs to be activated */
		FDSSkillSpecHandle Handle;

		/** True if this skill was activated remotely and needs to follow up, false if the skill hasn't been activated at all yet */
		bool bPartiallyActivated;

		FPendingSkillInfo()
			: bPartiallyActivated(false)
		{}
	};

	/** 
	* 현재 실행중인 Skill들의 정보들 (복제되는 데이터)
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ActivatableSkills, BlueprintReadOnly, Transient, Category = "Skills")
	FDSSkillSpecContainer ActivatableSkills;

	// 현재 실행중인 Skill object
	TArray<TObjectPtr<UDSSkillBase>>	ActiveSkills;

	UPROPERTY()
	TArray<TObjectPtr<UDSSkillBase>>	AllReplicatedInstancedSkills;
	TArray<TObjectPtr<UDSSkillBase>>& GetReplicatedInstancedSkills_Mutable() { return AllReplicatedInstancedSkills; }

	/** This is a list of GameplayAbilities that were activated on the server and can't yet execute on the client. It will try to execute these at a later point */
	TArray<FPendingSkillInfo> PendingServerActivatedSkills;

	UPROPERTY()
	bool bCachedIsNetSimulated;

	FTimerHandle    OnRep_ActivateSkillsTimerHandle;

	/** Tracks abilities that are blocked based on input binding. An skill is blocked if BlockedAbilityBindings[InputID] > 0 */
	UPROPERTY(Transient, Replicated)
	TArray<uint8> BlockedSkillBindings;

	uint8 bSkillPendingClearAll : 1;

	// ----------------------------------------------------------------------------------------------------------------
	//	Clear Skill
	// ----------------------------------------------------------------------------------------------------------------	
	void CheckForClearedSkills();

	// ----------------------------------------------------------------------------------------------------------------
	//	Actor interaction
	// ----------------------------------------------------------------------------------------------------------------	
private:
	/** The actor that owns this component logically */
	UPROPERTY(ReplicatedUsing = OnRep_SkillOwnerActor)
	TObjectPtr<AActor> SkillOwnerActor;

	/** The actor that is the physical representation used for abilities. Can be NULL */
	UPROPERTY(ReplicatedUsing = OnRep_SkillAvatarActor)
	TObjectPtr<AActor> SkillAvatarActor;
};
