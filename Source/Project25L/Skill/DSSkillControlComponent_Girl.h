// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/DSSkillControlComponent.h"
#include "DSSkillControlComponent_Girl.generated.h"

/**
 * 
 */
 class ADSCharacter_Girl;

UCLASS()
class PROJECT25L_API UDSSkillControlComponent_Girl : public UDSSkillControlComponent
{
	GENERATED_BODY()

public:
	UDSSkillControlComponent_Girl(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void InitSkillActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override; 

protected:
	void ActivateComboSkill();
	void ActivateFlightSkill();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Unreliable)
	virtual void ServerRPC_PlayComboMontage(int32 ComboIndex);
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void MulticastRPC_PlayComboMontage(int32 ComboIndex);

	UPROPERTY(Replicated)
	uint8 CanCombo:1;

	UPROPERTY(Replicated)
	uint8 bNextComboInputOn : 1;

	void PlayPunchComboAnimation(int32 ComboIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDSComboActionData> PunchComboActionData;

	UPROPERTY(Replicated)
	int32 CurrentCombo = 0;
	
	float ComboSpeedRate = 1.0f;
	const float AttackRange = 40.f;
	const float AttackRadius = 50.f;
	const float AttackDamage = 30.f;
};
