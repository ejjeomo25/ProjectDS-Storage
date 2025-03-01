// Default
#include "Item/DSItemActor.h"

// UE
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Game
#include "DSLogChannels.h"
#include "Character/DSCharacter.h"

ADSItemActor::ADSItemActor()
	: Super()
	, ItemID(0)
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));

	Trigger->SetupAttachment(Mesh);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADSItemActor::OnComponentBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ADSItemActor::OnComponentEndOverlap);
}

void ADSItemActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UI 띄우는 작업 ('F'키를 눌러라.. 등등)
	DS_LOG(DSItemLog, Log, TEXT("Item Begin Overlap"));

	//아이템을 저장한다.
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		Character->SetSurroundingItem(this);
	}

}

void ADSItemActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UI 띄우는 작업이 사라짐
	DS_LOG(DSItemLog, Log, TEXT("End Overlap"));

	//아이템을 리셋한다.
	ADSCharacter* Character = Cast<ADSCharacter>(OtherActor);

	if (IsValid(Character))
	{
		Character->SetSurroundingItem(nullptr);
	}
}



