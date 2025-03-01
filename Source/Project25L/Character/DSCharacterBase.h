#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/Character.h"

//UHT
#include "DSCharacterBase.generated.h"

class UDSSkillControlComponent;
class UDSStatComponent;

UCLASS()
class PROJECT25L_API ADSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADSCharacterBase(const FObjectInitializer& ObjectInitializer);

public:
	void OnTest();
	UDSSkillControlComponent* GetSkillControlComponent() const { return SkillControlComponent; }
	UDSStatComponent* GetStatComponent() const { return StatComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitSkillActorInfo();

	virtual void OnRep_Controller() override;

	//초기화하는 용도로, SkillControl/Stat/Mesh 등 초기화 할 함수이다.
	virtual void Initialize();

	//void PlayEffect(TSubclassOf)
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkillControl")
	TObjectPtr<UDSSkillControlComponent> SkillControlComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<UDSStatComponent> StatComponent;

};
