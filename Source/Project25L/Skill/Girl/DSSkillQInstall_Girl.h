#pragma once
// Default
#include "CoreMinimal.h"

// UE
// Game
#include "GameData/DSEnums.h"
#include "Skill/Base/DSSkillBase.h"

// UHT
#include "DSSkillQInstall_Girl.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class ADSWeapon;

UCLASS(Blueprintable)
class PROJECT25L_API UDSSkillQInstall_Girl : public UDSSkillBase
{
	GENERATED_BODY()

public:
	UDSSkillQInstall_Girl(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

protected:

	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled);

protected:
	UPROPERTY(Transient)
	FVector TargetLocation;

	UPROPERTY(Transient)
	uint8 bIsStarted : 1;
	
	UPROPERTY(Transient)
	TObjectPtr<ADSWeapon> Weapon;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> NiagaraEffect;

	UPROPERTY(EditAnywhere, Category = "DSSettings | Effect")
	TMap<ESkillActivationStatus, TSoftObjectPtr<UNiagaraSystem>> Effects;

};
