#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
    Girl,
    Boy,
    Mister,
};

UENUM(BlueprintType)
enum class EDataTableType : uint8
{
    CharacterData,
	NonCharacterData,
	SkillAttributeData,
	ItemData,
	ItemVehicleData,
	ItemPotionData,
	ItemGrenadeData,
	ItemAccessoryData,
};
// 몬스터 타입을 정의한다.
UENUM()
enum class EMonsterType : int32
{
    None = 0,
	Weak = 100,
	Strong = 200,
	Boss = 400,
	Max = 500
};

// 던전 타입
UENUM(BlueprintType)
enum class EDungeonType : uint8
{
	DungeonDoor1,
	DungeonDoor2,
	DungeonDoor3,
	DungeonDoor4,
	DungeonDoor5
};

/*
HUD에 띄울 대표적인 위젯
HUBMainWidget : HUB 상에서 등장하는 위젯
DungeonMainWidget : Dungeon 상에서 등장하는 위젯
*/
UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	HUBMainWidget,
	DungeonMainWidget
};


////////////////////////////////////////////////////////////////////// Skill enum 정리 //////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Skill1,						//QSkill,
	Skill2,						//ESKill,
	FarmingSkill,			// RSkill,
	UltimateSkill,			// XSkill, 
	MouseLSkill, 			//MouseLSkill,
	MouseLHoldSkill, 		//MouseLHoldSkill,
	MouseRSkill,			//MouseRSkill,
};

/**
 * 	@스킬이 실행될 때 어떻게 인스턴스화 되는지 결정.
	@스킬이 어떤 기능을 수행할 수 있는지 제한한다.
	@스킬이 개별 객체로 관리되는 방식(공유/복제/상태 저장 여부)을 결정
 */
UENUM(BlueprintType)
enum class ESkillInstancingPolicy : uint8
{
	 // 하나의 CDO 공유
	 // 패시브 스킬, 공식 계산
	 // 복제 X, 상태 저장 X
	NonInstanced,

	// 캐릭터마다 개별 생성
	// 버프, 쿨다운이 있는 액티브 스킬
	// 복제 O, 상태 저장O
	InstancedPerActor,

	// 실행할 때마다 생성
	// 투사체, 독립적인 소환 오브젝트
	// 복제 X, 상태 저장 O
	InstancedPerExecution,
};

/**
 * @스킬의 네트워크 실행 방식을 결정
   @어떤 환경(클라이언트/서버)에서 스킬이 실행되고, 실행 결과가 네트워크를 통해 어떻게 전파되는지를 정의
 */
UENUM(BlueprintType)
enum class ESkillNetExecutionPolicy :uint8
{
	LocalPredicted,  // 클라이언트에서 먼저 실행하고, 서버에서 최종 확정
	LocalOnly,       // 클라이언트에서만 실행, 네트워크 복제 없음
	ServerInitiated, // 서버에서 실행되며, 클라이언트에도 영향을 줌
	ServerOnly       // 서버에서만 실행되며, 클라이언트는 결과만 받음
};

/**
 * 	@스킬의 네트워크 복제 방식을 결정
	@스킬 인스턴스가 네트워크에서 어떻게 Replicate되는지 정의
 */
UENUM(BlueprintType)
enum class ESkillReplicationPolicy : uint8
{
	
	ReplicateNo	UMETA(DisplayName = "Do Not Replicate"),

	// 	스킬 인스턴스를 소유자(Owner)에게만 복제
	ReplicateYes	UMETA(DisplayName = "Replicate"),
};


UENUM(BlueprintType)
enum class  ESkillNetSecurityPolicy :uint8
{
	/** What protections does this ability have? Should the client be allowed to request changes to the execution of the ability? */
	// No security requirements. Client or server can trigger execution and termination of this ability freely.
	ClientOrServer			UMETA(DisplayName = "Client Or Server"),

	// A client requesting execution of this ability will be ignored by the server. Clients can still request that the server cancel or end this ability.
	ServerOnlyExecution		UMETA(DisplayName = "Server Only Execution"),

	// A client requesting cancellation or ending of this ability will be ignored by the server. Clients can still request execution of the ability.
	ServerOnlyTermination	UMETA(DisplayName = "Server Only Termination"),

	// Server controls both execution and termination of this ability. A client making any requests will be ignored.
	ServerOnly				UMETA(DisplayName = "Server Only"),
};

/**
 * 	@캐릭터의 이동 속도 유형을 정의
 */
UENUM(BlueprintType)
enum class ESpeedType : uint8
{
	None = 0x00 UMETA(DisplayName = "None"),
    Forward = 0x10 UMETA(DisplayName = "Forward"),
    Backward = 0x20 UMETA(DisplayName = "Backward"),
    Sprint = 0x40 UMETA(DisplayName = "Sprint")
};

/**
 * 	@웅크리기(Crouch) 모드를 정의
 */
UENUM(BlueprintType)
enum class ECrouchMode : uint8
{
	None UMETA(DisplayName = "None"),
	HoldMode UMETA(DisplayName = "HoldMode"),
	ToggleMode UMETA(DisplayName = "ToggleMode")
};


UENUM()
enum class EItemType 
{
	None = 0,
	Vehicle = 100,
	Potion = 200,
	Grenade = 300,
	Accessory = 400,
	Max = 500,
};

/**
 * @brief 캐릭터 스탯 유형 열거형
 *
 * 캐릭터의 다양한 능력치를 식별하기 위한 열거형입니다.
 * @note 캐릭터 현재 체력(HP)은 StatComponent에서 별도로 관리됩니다.
 * @see UDSStatComponent, FDSCharacterStat
 */
UENUM(BlueprintType)
enum class EDSStatType : uint8
{
	MaxHP,				///< 최대 체력 (기본 생명력)
	Attack,				///< 공격력
	Defense,			///< 방어력 (받는 데미지 감소)
	Luck,				///< 행운 (치명타 확률 증가, 공격 피해량 증가, 받는 피해량 감소, 스킬 성능 증가)
	MoveSpeed,			///< 이동 속도 배율 (*)
	AttackSpeed,		///< 초당 공격 횟수
	AttackRange,		///< 평타 공격 사거리 (미터)
	HPRegen,			///< 초당 체력 재생량
	HPSteal,			///< 공격 시 데미지 대비 흡혈 비율 (%)
	DamageBoost,		///< 주는 모든 데미지의 증가율 (%)
	DamageReduction,	///< 받는 데미지 감소율 (%)
	CDReduction,		///< 스킬 쿨타임 감소 시간 (초)
	CritChance,			///< 치명타 발생 확률 (%)
	CritDamage,			///< 치명타 발생 시 데미지 증가율 (%)
};

/**
 * @brief 수치 연산 방식 열거형
 *
 * @note Stat 계산에 사용되고 있습니다.
 * @see UDSStatComponent
 */
UENUM(BlueprintType)
enum class EOperationType : uint8
{
	None,				///< 연산 타입 미지정
	Additive,			///< 덧셈 연산 (예: +10, -20)
	Multiplicative,		///< 배율 연산 (예: 20% 증가는 0.2)
	Percent,			///< 퍼센트 연산 (예: +10%, -20%)
};

/**
 * @brief 데미지 소스 유형 열거형
 *
 * 공격 데미지 계산 시 데미지의 원천을 구분합니다.
 * @see UDSStatComponent::ReceiveDamage
 */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None,				///< 데미지 타입 미지정
	BaseAttack,			///< 기본 공격 (평타) 데미지
	Skill,				///< 스킬 데미지
};

/**
 * @brief 힐 소스 유형 열거형
 *
 * 힐(치유) 효과의 원천을 구분하기 위한 열거형입니다.
 * @see UDSStatComponent::RecoverHealth
 */
UENUM(BlueprintType)
enum class EHealType : uint8
{
	None,				///< 힐 타입 미지정
	Potion,				///< 포션 아이템 사용
	Skill,				///< 스킬 회복
};

/**
 * @brief 원소 유형 열거형
 *
 * 원소 상성 시스템을 위한 열거형입니다.
 * @see UDSStatComponent::ReceiveDamage
 */
UENUM(BlueprintType)
enum class EDSElementType : uint8
{
	None,				///< 중립 속성 (상성 영향 없음)
	Fire,				///< 불 속성
	Ice,				///< 얼음 속성
	Earth,				///< 대지 속성
};

/**
 * @brief 스포너 종류
 * 
 * Range : 랜덤 범위에서 스폰된다.
 * Fixed : 아이템 박스가 고정된 위치에서 스폰되어지고, 
*  정규분포로 들어가진 아이템이 결정되어진다.
 */
UENUM(BlueprintType)
enum class ESpawnerType : uint8
{
	RangeItem,
	FixedItem,
	RangeMonster,
};
