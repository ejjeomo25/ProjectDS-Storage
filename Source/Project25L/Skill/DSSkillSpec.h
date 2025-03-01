#pragma once

//Default
#include "CoreMinimal.h"

//UE
#include "UObject/NoExportTypes.h"
#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"

//Game
#include "DSSkillSpec.generated.h"

class UDSSkillBase;
class UDSSkillControlComponent;

 /** Handle that points to a specific granted ability. These are globally unique */
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpecHandle
{
	GENERATED_BODY()

	FDSSkillSpecHandle()
		: Handle(INDEX_NONE)
	{
	}

	bool IsValid() const
	{
		return Handle != INDEX_NONE;
	}

	void GenerateNewHandle();

	bool operator==(const FDSSkillSpecHandle& Other) const
	{
		return Handle == Other.Handle;
	}

	bool operator!=(const FDSSkillSpecHandle& Other) const
	{
		return Handle != Other.Handle;
	}

	friend uint32 GetTypeHash(const FDSSkillSpecHandle& SpecHandle)
	{
		return ::GetTypeHash(SpecHandle.Handle);
	}

	FString ToString() const
	{
		return IsValid() ? FString::FromInt(Handle) : TEXT("Invalid");
	}

private:
	UPROPERTY()
	int32 Handle;
};

/*
*/
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpec : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	FDSSkillSpec(const FDSSkillSpec&) = default;
	FDSSkillSpec(FDSSkillSpec&&) = default;
	FDSSkillSpec& operator=(const FDSSkillSpec&) = default;
	FDSSkillSpec& operator=(FDSSkillSpec&&) = default;
	~FDSSkillSpec() = default;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

		FDSSkillSpec()
		: Skill(nullptr), InputID(INDEX_NONE), SourceObject(nullptr), ActiveCount(0), InputPressed(false), RemoveAfterActivation(false), PendingRemove(false), bActivateOnce(false)
	{ }

	/** Version that takes an ability class */
	FDSSkillSpec(TSubclassOf<UDSSkillBase> InSkillClass,int32 InInputID = INDEX_NONE, UObject* InSourceObject = nullptr);

	/** Version that takes an ability CDO, this exists for backward compatibility */
	FDSSkillSpec(UDSSkillBase* InSkill,int32 InInputID = INDEX_NONE, UObject* InSourceObject = nullptr);

	/** Handle for outside sources to refer to this spec by */
	UPROPERTY()
	FDSSkillSpecHandle Handle;
	
	/** Ability of the spec (Always the CDO. This should be const but too many things modify it currently) */
	UPROPERTY()
	TObjectPtr<UDSSkillBase> Skill;
	
	/** InputID, if bound */
	UPROPERTY()
	int32	InputID;

	/** Object this ability was created from, can be an actor or static object. Useful to bind an ability to a gameplay object */
	UPROPERTY()
	TWeakObjectPtr<UObject> SourceObject;

	/** A count of the number of times this ability has been activated minus the number of times it has been ended. For instanced abilities this will be the number of currently active instances. Can't replicate until prediction accurately handles this.*/
	UPROPERTY(NotReplicated)
	uint8 ActiveCount;

	/** Is input currently pressed. Set to false when input is released */
	UPROPERTY(NotReplicated)
	uint8 InputPressed:1;

	/** If true, this ability should be removed as soon as it finishes executing */
	UPROPERTY(NotReplicated)
	uint8 RemoveAfterActivation:1;

	/** Pending removal due to scope lock */
	UPROPERTY(NotReplicated)
	uint8 PendingRemove:1;

	/** This ability should be activated once when it is granted. */
	UPROPERTY(NotReplicated)
	uint8 bActivateOnce : 1;

	/** Non replicating instances of this ability. */
	UPROPERTY(NotReplicated)
	TArray<TObjectPtr<UDSSkillBase>> NonReplicatedInstances;

	/** Replicated instances of this ability.. */
	UPROPERTY()
	TArray<TObjectPtr<UDSSkillBase>> ReplicatedInstances;

	/**
	 * Handle to GE that granted us (usually invalid). FActiveGameplayEffectHandles are not synced across the network and this is valid only on Authority.
	 * If you need FGameplayAbilitySpec -> FActiveGameplayEffectHandle, then use AbilitySystemComponent::FindActiveGameplayEffectHandle.
	 */

	/** Returns the primary instance, used for instance once abilities */
	UDSSkillBase* GetPrimaryInstance() const;

	/** interface function to see if the ability should replicated the ability spec or not */
	bool ShouldReplicateSkillSpec() const;

	/** Returns all instances, which can include instance per execution abilities */
	TArray<UDSSkillBase*> GetSkillInstances() const
	{
		TArray<UDSSkillBase*> Skills;
		Skills.Append(ReplicatedInstances);
		Skills.Append(NonReplicatedInstances);
		return Skills;
	}

	/** Returns true if this ability is active in any way */
	bool IsActive() const;

	void PreReplicatedRemove(const struct FDSSkillSpecContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FDSSkillSpecContainer& InArraySerializer);

	FString GetDebugString();
};

/** Fast serializer wrapper for above struct */
USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillSpecContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FDSSkillSpecContainer()
		: Owner(nullptr)
	{
	}

	/** List of activatable Skills */
	UPROPERTY()
	TArray<FDSSkillSpec> Items;

	/** Component that owns this list */
	UPROPERTY(NotReplicated)
	TObjectPtr<UDSSkillControlComponent> Owner;

	/** Initializes Owner variable */
	void RegisterWithOwner(UDSSkillControlComponent* InOwner);

	/* 복제된 데이터만 전달 */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FDSSkillSpec, FDSSkillSpecContainer>(Items, DeltaParms, *this);
	}

	template< typename Type, typename SerializerType >
	bool ShouldWriteFastArrayItem(const Type& Item, const bool bIsWritingOnClient)
	{
		// if we do not want the FGameplayAbilitySpec to replicated return false;
		if (!Item.ShouldReplicateSkillSpec())
		{
			return false;
		}

		if (bIsWritingOnClient)
		{
			return Item.ReplicationID != INDEX_NONE;
		}

		return true;
	}
};

template<>
struct TStructOpsTypeTraits< FDSSkillSpecContainer> : public TStructOpsTypeTraitsBase2< FDSSkillSpecContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct PROJECT25L_API FDSSkillDef
{
	FDSSkillDef()
		: InputID(INDEX_NONE)
		, SourceObject(nullptr)
	{ }
	
	GENERATED_BODY()

	/** What Skill to grant */
	UPROPERTY(EditDefaultsOnly, Category="Skill Definition", NotReplicated)
	TSubclassOf<UDSSkillBase> Skill;

	/** Input ID to bind this Skill to */
	UPROPERTY(EditDefaultsOnly, Category="Skill Definition", NotReplicated)
	int32 InputID;

	/** What granted this spec, not replicated or settable in editor */
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UObject> SourceObject;

	/** This handle can be set if the SpecDef is used to create a real FGameplaybilitySpec */
	 UPROPERTY()
	 FDSSkillSpecHandle AssignedHandle;

	bool operator==(const FDSSkillDef& Other) const;
	bool operator!=(const FDSSkillDef& Other) const;
};