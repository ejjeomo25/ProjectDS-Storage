//Defualt
#include "Stat/DSStatComponent.h"

//UE
#include "Net/UnrealNetwork.h"

//Game
#include "GameData/DSGameDataSubsystem.h"
#include "DSLogChannels.h"

/**
 * @brief 각 스탯 유형과 FDSCharacterStat의 멤버 변수 포인터를 매핑하는 맵
 *
 * EDSStatType 열거형에 해당하는 스탯 멤버를 FDSCharacterStat 구조체에서 쉽게 접근하기 위한 매핑 테이블입니다.
 */
static const TMap<EDSStatType, float FDSCharacterStat::*> StatMemberMap =
{
	{EDSStatType::MaxHP,          &FDSCharacterStat::MaxHP},
	{EDSStatType::Attack,         &FDSCharacterStat::Attack},
	{EDSStatType::Defense,        &FDSCharacterStat::Defense},
	{EDSStatType::Luck,           &FDSCharacterStat::Luck},
	{EDSStatType::MoveSpeed,      &FDSCharacterStat::MoveSpeed},
	{EDSStatType::AttackSpeed,    &FDSCharacterStat::AttackSpeed},
	{EDSStatType::AttackRange,    &FDSCharacterStat::AttackRange},
	{EDSStatType::HPRegen,        &FDSCharacterStat::HPRegen},
	{EDSStatType::HPSteal,        &FDSCharacterStat::HPSteal},
	{EDSStatType::DamageBoost,    &FDSCharacterStat::DamageBoost},
	{EDSStatType::DamageReduction,&FDSCharacterStat::DamageReduction},
	{EDSStatType::CDReduction,    &FDSCharacterStat::CDReduction},
	{EDSStatType::CritChance,     &FDSCharacterStat::CritChance},
	{EDSStatType::CritDamage,     &FDSCharacterStat::CritDamage},
};

UDSStatComponent::UDSStatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NextBuffID(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDSStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 기본 스탯을 초기화합니다.
	InitializeStats();
}

void UDSStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDSStatComponent, CurrentStat);
	DOREPLIFETIME(UDSStatComponent, HP);
}

void UDSStatComponent::ModifyHP(float Delta)
{
	HP += Delta;
	
	if (HP > CurrentStat.MaxHP)
	{
		HP = CurrentStat.MaxHP;
		DS_LOG(LogTemp, Log, TEXT("Over MaxHP"));
	}
	
	if (HP <= 0.f)
	{
		HP = 0.f;
		DS_LOG(LogTemp, Log, TEXT("Die"));
	}
	
	DSEVENT_DELEGATE_INVOKE(GameEvent.OnHPChanged, HP);

	DS_LOG(LogTemp, Log, TEXT("[%s] [%s] HP 변경: %+f, 새 HP: %f"), *GetOwner()->GetName(), *GetName(), Delta, HP);
}

void UDSStatComponent::OnRep_CurrentStat()
{

}

void UDSStatComponent::OnRep_HP()
{
}

void UDSStatComponent::InitializeStats()
{
	UWorld* World = GetWorld();
	check(World);

	UDSGameDataSubsystem* DataSubsystem = UDSGameDataSubsystem::Get(this);
	check(DataSubsystem);

	FDSCharacterStat* CharacterStatData = DataSubsystem->GetDataRow<FDSCharacterStat, ECharacterType>(EDataTableType::CharacterData, CharcterType);
	if (nullptr != CharacterStatData)
	{
		DefaultStat = *CharacterStatData;
		CurrentStat = DefaultStat;

		HP = CurrentStat.MaxHP;

		if (HasAuthority())
		{
			OnRep_CurrentStat();
			OnRep_HP();
		}
	}
	else
	{
		DS_LOG(LogTemp, Warning, TEXT("Requested Character Data for type %s not found!"), *UEnum::GetValueAsString(CharcterType));
	}
}

float UDSStatComponent::GetDefaultStatByEnum(EDSStatType StatType) const
{
	float FDSCharacterStat::* const* MemberPtr = StatMemberMap.Find(StatType);
	if (nullptr != MemberPtr)
	{
		return DefaultStat.*(*MemberPtr);
	}
	
	return 0.0f;
}

float UDSStatComponent::GetFinalStat(EDSStatType StatType) const
{
	float TotalAddend = 0.f;
	float TotalMultiplier = 0.f;

	for (const FBuffEntry& Entry : ActiveBuffs)
	{
		if (Entry.StatType == StatType)
		{
			if (Entry.OperationType == EOperationType::Additive)
			{
				TotalAddend += Entry.BuffValue;
			}
			else if (Entry.OperationType == EOperationType::Multiplicative)
			{
				TotalMultiplier += Entry.BuffValue;
			}
			else if (Entry.OperationType == EOperationType::Percent)
			{
				TotalMultiplier += (Entry.BuffValue / 100);
			}
		}
	}

	// TODO : Item Stat 추가

	// 최종 스탯 = (기본 스탯 + Additive 버프) * (1 + Multiplicative 버프)
	float DefaultStatValue = GetDefaultStatByEnum(StatType);
	float FinalStat = (DefaultStatValue + TotalAddend) * (1.f + TotalMultiplier);
	return FinalStat > 0.f ? FinalStat : 0.f;
}

void UDSStatComponent::UpdateCurrentStat()
{
	for (const TPair<EDSStatType, float FDSCharacterStat::*>& Pair : StatMemberMap)
	{
		const EDSStatType& StatType = Pair.Key;
		float FDSCharacterStat::* MemberPtr = Pair.Value;
		CurrentStat.*MemberPtr = GetFinalStat(StatType);
	}
	
	PostUpdateCurrentStat();
}

void UDSStatComponent::PostUpdateCurrentStat()
{
	// MaxHP 값 변경에 대한 HP 값 변경
	ModifyHP(0);
}

void UDSStatComponent::ApplyBuff(EDSStatType InStatType, EOperationType InOperationType, float InBuffValue, float InDuration)
{
	// 새로운 버프 항목 생성 및 추가
	FBuffEntry NewEntry;
	NewEntry.StatType = InStatType;
	NewEntry.OperationType = InOperationType;
	NewEntry.BuffID = NextBuffID++;
	NewEntry.BuffValue = InBuffValue;
	ActiveBuffs.Add(NewEntry);

	DS_LOG(LogTemp, Log, TEXT("[%s] %s 버프 적용: ID=%d, %s %f, 지속시간: %f초"), *GetName(), *UEnum::GetValueAsString(InStatType), NewEntry.BuffID, *UEnum::GetValueAsString(InOperationType), InBuffValue, InDuration);

	// 버프 제거 타이머 설정
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UDSStatComponent::RemoveBuff, NewEntry.BuffID);
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, InDuration, false);
		BuffTimerHandles.Add(NewEntry.BuffID, TimerHandle);
	}

	UpdateCurrentStat();
}

void UDSStatComponent::RemoveBuff(int32 InBuffID)
{
	// 버프 ID를 기준으로 해당 항목을 검색 후 제거
	int32 RemovedIndex = ActiveBuffs.IndexOfByPredicate([InBuffID](const FBuffEntry& Entry)
		{
			return Entry.BuffID == InBuffID;
		});

	if (RemovedIndex != INDEX_NONE)
	{
		float RemovedBuffValue = ActiveBuffs[RemovedIndex].BuffValue;
		EDSStatType RemovedBuffStatType = ActiveBuffs[RemovedIndex].StatType;
		EOperationType RemovedBuffOperationType = ActiveBuffs[RemovedIndex].OperationType;
		DS_LOG(LogTemp, Log, TEXT("[%s] %s 버프 제거: ID=%d, %s %f"), *GetName(), *UEnum::GetValueAsString(RemovedBuffStatType), InBuffID, *UEnum::GetValueAsString(RemovedBuffOperationType), RemovedBuffValue);
		ActiveBuffs.RemoveAt(RemovedIndex);
	}

	// 타이머 핸들 제거
	BuffTimerHandles.Remove(InBuffID);

	UpdateCurrentStat();
}

void UDSStatComponent::ReceiveDamage_Implementation(float DamageAmount, EDamageType DamageType, EDSElementType ElementType, AActor* Instigator)
{
	ModifyHP(-DamageAmount);
}

void UDSStatComponent::RecoverHealth_Implementation(float HealAmount, EHealType HealType, AActor* Instigator)
{
	ModifyHP(HealAmount);
}

