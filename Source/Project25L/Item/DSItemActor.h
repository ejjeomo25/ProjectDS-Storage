#pragma once
// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// UHT
#include "DSItemActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT25L_API ADSItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSItemActor();

	int32 GetID() { return ItemID; }
protected:                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	UPROPERTY(EditAnywhere, Category = Item)
	uint32 ItemID;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	TObjectPtr<UBoxComponent> Trigger;
};
