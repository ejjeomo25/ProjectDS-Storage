#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Components/Skill/DSSkillControlComponent.h"

// UHT
#include "DSSkillControlComponent_Girl.generated.h"

 class ADSCharacter_Girl;

UCLASS()
class PROJECT25L_API UDSSkillControlComponent_Girl : public UDSSkillControlComponent
{
	GENERATED_BODY()

public:
	UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void InstallSkillTimer(float Sec, bool bIsCanceled);
public:
	//시간 초과에 대한 델리게이트를 만든다.
	DECLARE_DELEGATE(FOnExpiredSkill)
	FOnExpiredSkill OnExpiredSkill;

protected:	

	virtual void ActivatePrimarySKill() override;

	virtual void ActivateSKill1() override;
	virtual void ActivateSKill2() override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(Transient)
	uint8 bHasInstalledSkill;

	UPROPERTY(Transient)
	FTimerHandle InstallationSkillTimerHandle;
};
