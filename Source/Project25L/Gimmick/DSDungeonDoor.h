
#pragma once

// Default
#include "CoreMinimal.h"

// UE
#include "GameFramework/Actor.h"

// Game
#include "System/DSEnums.h"

// UHT
#include "DSDungeonDoor.generated.h"

class ADSDungeonMat;
class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UDSDoorData;
class UGeometryCollectionComponent;

/*
* 입장 시 DungeonDoor - DungeonMat 같이 사용
* 퇴장 시 DungeonDoor만 사용
* HUB에서 이미 다녀온 던전은 Chaos Destuction을 사용해서 부서져 
* 있고, 문 Mesh의 중간에 Height/Width 2분의 1 위치에 구슬 Mesh를 띄운다. (그 전까진 숨기고 있음)
*/
UCLASS()
class PROJECT25L_API ADSDungeonDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADSDungeonDoor();

	void UnlockDoor(EDungeonType CurrentType);
	void DestroyDoor(EDungeonType CurrentType);

protected:

	void InitializeStage();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lock)
	bool bIsUnlocked;

	UPROPERTY(Transient)
	TObjectPtr<ADSDungeonMat> MatVolume;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<UStaticMeshComponent> DungeonBead;

	UPROPERTY(EditAnywhere, Category = Data)
	TObjectPtr<UDSDoorData> DungeonDoorData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	TObjectPtr<UBoxComponent> Trigger;

};
