
//Defualt
#include "Skill/DSSkillSpec.h"

//UE
#include "UObject/NoExportTypes.h"
#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"

//Game
#include "System/DSEnums.h"
#include "DSSkillBase.h"
#include "DSSkillControlComponent.h"


// ---------------------------------------------------- FDSSkillSpecHandle ----------------------------------------------------
void FDSSkillSpecHandle::GenerateNewHandle()
{
	// Must be in C++ to avoid duplicate statics accross execution units
	static int32 GHandle = 1;
	Handle = GHandle++;
}




// ---------------------------------------------------- FDSSkillSpec ----------------------------------------------------
FDSSkillSpec::FDSSkillSpec(TSubclassOf<UDSSkillBase> InSkillClass, int32 InInputID, UObject* InSourceObject)
	: Skill(InSkillClass ? InSkillClass.GetDefaultObject() : nullptr)
	, InputID(InInputID)
	, SourceObject(InSourceObject)
	, ActiveCount(0)
	, InputPressed(false)
	, RemoveAfterActivation(false)
	, PendingRemove(false)
	, bActivateOnce(false)
{
	Handle.GenerateNewHandle();
}

FDSSkillSpec::FDSSkillSpec(UDSSkillBase* InSkill,int32 InInputID, UObject* InSourceObject)
	: Skill(InSkill)
	, InputID(InInputID)
	, SourceObject(InSourceObject)
	, ActiveCount(0)
	, InputPressed(false)
	, RemoveAfterActivation(false)
	, PendingRemove(false)
	, bActivateOnce(false)
{
	Handle.GenerateNewHandle();
}

UDSSkillBase* FDSSkillSpec::GetPrimaryInstance() const
{
	if (Skill && Skill->GetInstancingPolicy() == ESkillInstancingPolicy::InstancedPerActor)
	{
		if (NonReplicatedInstances.Num() > 0)
		{
			return NonReplicatedInstances[0];
		}

		if (ReplicatedInstances.Num() > 0)
		{
			return ReplicatedInstances[0];
		}
	}
	return nullptr;
}

bool FDSSkillSpec::ShouldReplicateSkillSpec() const
{
	if (Skill && Skill->ShouldReplicateSkillSpec(*this))
	{
		return true;
	}

	return false;
}

bool FDSSkillSpec::IsActive() const
{
	return Skill != nullptr && ActiveCount > 0;
}

void FDSSkillSpec::PreReplicatedRemove(const FDSSkillSpecContainer& InArraySerializer)
{
	if (InArraySerializer.Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("PreReplicatedRemove called for Skill: %s, Owner : %s"), *GetDebugString(), *InArraySerializer.Owner->GetName());
		InArraySerializer.Owner->OnRemoveSkill(*this);
	}
}

void FDSSkillSpec::PostReplicatedAdd(const FDSSkillSpecContainer& InArraySerializer)
{
	if (InArraySerializer.Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("PostReplicatedAdd called for Skill: %s, Owner : %s"), *GetDebugString(), *InArraySerializer.Owner->GetName());
		InArraySerializer.Owner->OnAddSkill(*this);
	}
}

FString FDSSkillSpec::GetDebugString()
{
	return FString::Printf(TEXT("(%s)"), *GetNameSafe(Skill));
}


// ---------------------------------------------------- FDSSkillSpecContainer ----------------------------------------------------

void FDSSkillSpecContainer::RegisterWithOwner(UDSSkillControlComponent* InOwner)
{
	Owner = InOwner;
}


// ---------------------------------------------------- FDSSkillDef ----------------------------------------------------

bool FDSSkillDef::operator==(const FDSSkillDef& Other) const
{
	return Skill == Other.Skill &&
		//LevelScalableFloat == Other.LevelScalableFloat &&
		InputID == Other.InputID;// &&
		//RemovalPolicy == Other.RemovalPolicy;
}

bool FDSSkillDef::operator!=(const FDSSkillDef& Other) const
{
	return !(*this == Other);
}
