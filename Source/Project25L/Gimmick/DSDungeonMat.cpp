// Default
#include "Gimmick/DSDungeonMat.h"

// UE
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Game
#include "Character/DSCharacter.h"
#include "GameData/Gimmick/DSDoorData.h"
#include "Player/DSPlayerController.h"
#include "System/DSEventSystems.h"

#include "DSLogChannels.h"

ADSDungeonMat::ADSDungeonMat()
	: Super()
	, MaxPlayers(3)
	, WaitSec(3.f)
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
}

void ADSDungeonMat::BindEvents()
{
	if (IsValid(Trigger))
	{
		Trigger->OnComponentBeginOverlap.RemoveAll(this);
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSDungeonMat::OnComponentBeginOverlap);
		Trigger->OnComponentEndOverlap.RemoveAll(this);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSDungeonMat::OnComponentEndOverlap);
	}

	DSEVENT_DELEGATE_BIND(GameEvent.OnGimmickStateChanged, this, &ADSDungeonMat::OnGimmickStateChanged);

}

void ADSDungeonMat::UnbindEvents()
{
	if (IsValid(Trigger))
	{
		Trigger->OnComponentBeginOverlap.RemoveAll(this);
		Trigger->OnComponentEndOverlap.RemoveAll(this);
	}

	DSEVENT_DELEGATE_REMOVE(GameEvent.OnGimmickStateChanged, this);
}

void ADSDungeonMat::InitializeData(const FString& DoorDestination)
{
	Destination = DoorDestination;
}

void ADSDungeonMat::OnGimmickStateChanged(bool bShouldChange)
{
	Mesh->SetVisibility(bShouldChange);
}

void ADSDungeonMat::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		// 오버랩된 플레이어가 비어있다면, 처음 입장한 플레이어로 인식되어진다.
		if (OverlappedPlayers.IsEmpty())
		{
			//처음 입장하면, Mesh가 보인다.
			//하지만 DungeonMat는 현재 World에 배치되어 있기 때문에 Character의 도움을 통해서 Server -> Clients 전달해야한다.
			if (IsValid(Mesh) && HasAuthority())
			{
				DS_NETLOG(DSNetLog, Log, TEXT("First Overlap!"));
				Character->MulticastRPC_SetGimmickState(!Mesh->GetVisibleFlag());
			}
		}

		OverlappedPlayers.Add(Character);

		UWorld* World = GetWorld();

		check(World);

		if (OverlappedPlayers.Num() == MaxPlayers)
		{
			DS_NETLOG(DSNetLog, Log, TEXT("The timer works %d %d"),OverlappedPlayers.Num(), MaxPlayers);
			//시간초가 작동한다.
			World->GetTimerManager().SetTimer(WaitTimer, this, &ADSDungeonMat::PrepareDungeonTravel, WaitSec, false);
		}
	}
}

void ADSDungeonMat::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		UWorld* World = GetWorld();

		check(World);

		//한 명의 플레이어라도 나갈 경우 타이머를 초기화한다.
		World->GetTimerManager().ClearTimer(WaitTimer);

		for (const auto OverlappedPlayer : OverlappedPlayers)
		{
			if (Character == OverlappedPlayer)
			{
				OverlappedPlayers.Remove(Character);
				break;
			}
		}

		if (OverlappedPlayers.IsEmpty() && IsValid(Mesh) && HasAuthority())
		{
			//비어있으면, 모든 플레이어가 나갔기 때문에 매트를 없앤다.
			DS_NETLOG(DSNetLog, Log, TEXT("Last Overlap!"));
			Character->MulticastRPC_SetGimmickState(!Mesh->GetVisibleFlag());
		}
	}
}

void ADSDungeonMat::PrepareDungeonTravel()
{

	// Travel로 이동하기 전에 Cinematic or Animation 재생 되어진다.
	// PlayerController로부터 부탁하여 재생될 수 있도록 한다.
	// Cinematic or Animation이 끝날 때 실제 ServerTravel이 이루어진다.

	for (const auto OverlappedPlayer : OverlappedPlayers)
	{
		ADSPlayerController* PC = OverlappedPlayer->GetController<ADSPlayerController>();

		if (IsValid(PC))
		{
			PC->PlayDoorTransition();
		}
	}

	if (HasAuthority())
	{
		UWorld* World = GetWorld();

		check(World);

		FTimerHandle TravelWaitTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(TravelWaitTimerHandle, this, &ADSDungeonMat::TravelDungeon, 5.0f, false);
	}

}

void ADSDungeonMat::TravelDungeon()
{
	UWorld* World = GetWorld();

	check(World);

	World->ServerTravel(Destination);

}
