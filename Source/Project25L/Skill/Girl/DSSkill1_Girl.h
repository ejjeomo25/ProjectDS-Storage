#pragma once

// Default
#include "CoreMinimal.h"

// Game
#include "Skill/Base/DSSkillBase.h"

//UHT
#include "DSSkill1_Girl.generated.h"



UCLASS()
class PROJECT25L_API UDSSkill1_Girl : public UDSSkillBase
{
	GENERATED_BODY()

public:
	UDSSkill1_Girl(const FObjectInitializer& ObjectInitalize);

	void ExpireSkill();
protected:
	// 스킬의 핵심 기능을 실행하며, 파생 클래스에서 오버라이드하여 구체적인 로직을 구현하는 함수
	virtual void ActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);
	virtual void CallActivateSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo);

	// 스킬 실행 완료 후 종료를 알리고 후처리(자원 해제, 상태 초기화 등)를 수행 하는 함수
	virtual void EndSkill(const FDSSkillSpecHandle Handle, const FDSSkillActorInfo* ActorInfo, bool  bReplicateEndSkill, bool bWasCancelled);

protected:
	virtual void PostInitProperties() override;
protected:
	UPROPERTY(Transient)
	FVector TargetLocation;

	UPROPERTY(Transient)
	uint8 bHasInstalledSkill;
};
