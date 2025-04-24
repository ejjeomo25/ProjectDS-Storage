#pragma once
//Default
#include "CoreMinimal.h"

//UE
#include "GameFramework/Character.h"

//Game
#include "GameData/DSEnums.h"

//UHT
#include "DSCharacterBase.generated.h"

class UDSSkillControlComponent;
class UDSStatComponent;
struct FDSDamageEvent;

UCLASS()
class PROJECT25L_API ADSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADSCharacterBase(const FObjectInitializer& ObjectInitializer);

	UDSSkillControlComponent* GetSkillControlComponent() const { return SkillControlComponent; }
	UDSStatComponent* GetStatComponent() const { return StatComponent; }
public:
	virtual float TakeFinalDamage(float DamageAmount, const FDSDamageEvent& NewDamageEvent, class AController* EventInstigator, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

protected:

	virtual void InitSkillActorInfo();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Skill", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSSkillControlComponent> SkillControlComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DSSettings | Stat", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDSStatComponent> StatComponent;

};
