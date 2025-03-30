//Defualt
#include "Skill/DSSkillActorInfo.h"

//Game
#include "DSSkillControlComponent.h"
#include "DSLogChannels.h"
#include "GameFramework/MovementComponent.h"

void FDSSkillActorInfo::SetCharacterActor(AActor* NewCharacterActor)
{
	InitFromActor(SkillOwner.Get(), NewCharacterActor, SkillControlComponent.Get());
}

void FDSSkillActorInfo::ClearActorInfo()
{
	SkillOwner = nullptr;
	SkillAvatar = nullptr;
	PlayerController = nullptr;
	SkeletalMeshComponent = nullptr;
	MovementComponent = nullptr;
}

void FDSSkillActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UDSSkillControlComponent* InSkillControlComponent)
{
	check(InOwnerActor);
	check(InSkillControlComponent);

	SkillOwner = InOwnerActor;
	SkillAvatar = InAvatarActor;
	SkillControlComponent = InSkillControlComponent;

	// DS_LOG(DSSkillLog, Warning, TEXT("AffectedAnimInstanceTag = %s"), *AffectedAnimInstanceTag.ToString());

	AffectedAnimInstanceTag = InSkillControlComponent->AffectedAnimInstanceTag;

	APlayerController* OldPC = PlayerController.Get();

	// Look for a player controller or pawn in the owner chain.
	AActor* TestActor = InOwnerActor;
	while (TestActor)
	{
		if (APlayerController* CastPC = Cast<APlayerController>(TestActor))
		{
			PlayerController = CastPC;
			break;
		}

		if (APawn* Pawn = Cast<APawn>(TestActor))
		{
			PlayerController = Cast<APlayerController>(Pawn->GetController());
			break;
		}

		TestActor = TestActor->GetOwner();
	}

	// Notify ASC if PlayerController was found for first time
	if (OldPC == nullptr && PlayerController.IsValid())
	{
		InSkillControlComponent->OnPlayerControllerSet();
	}

	if (AActor* const AvatarActorPtr = SkillAvatar.Get())
	{
		// Grab Components that we care about
		SkeletalMeshComponent = AvatarActorPtr->FindComponentByClass<USkeletalMeshComponent>();
		MovementComponent = Cast<UMovementComponent>(AvatarActorPtr->FindComponentByClass<UMovementComponent>());
	}
	else
	{
		SkeletalMeshComponent = nullptr;
		MovementComponent = nullptr;
	}
}

UAnimInstance* FDSSkillActorInfo::GetAnimInstance() const
{
	const USkeletalMeshComponent* SKMC = SkeletalMeshComponent.Get();

	if (SKMC)
	{
		if (AffectedAnimInstanceTag != NAME_None)
		{
			if (UAnimInstance* Instance = SKMC->GetAnimInstance())
			{
				return Instance->GetLinkedAnimGraphInstanceByTag(AffectedAnimInstanceTag);
			}
		}

		return SKMC->GetAnimInstance();
	}

	return nullptr;
}

bool FDSSkillActorInfo::IsLocallyControlled() const
{
	if (const APlayerController* PC = PlayerController.Get())
	{
		return PC->IsLocalController();
	}
	else if (const APawn* OwnerPawn = Cast<APawn>(SkillOwner))
	{
		if (OwnerPawn->IsLocallyControlled())
		{
			return true;
		}
		else if (OwnerPawn->GetController())
		{
			// We're controlled, but we're not locally controlled.
			return false;
		}
	}

	return IsNetAuthority();
}

bool FDSSkillActorInfo::IsLocallyControlledPlayer() const
{
	if (const APlayerController* PC = PlayerController.Get())
	{
		return PC->IsLocalController();
	}

	return false;
}

bool FDSSkillActorInfo::IsNetAuthority() const
{
	// Make sure this works on pending kill actors
	AActor* const OwnerActorPtr = SkillOwner.Get(/*bEvenIfPendingKill=*/ true);
	if (OwnerActorPtr)
	{
		return (OwnerActorPtr->GetLocalRole() == ROLE_Authority);
	}

	return false;
}


