// Default
#include "AI/Task/BTTaskNode_FindPatrolPos.h"

// UE
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

// Game

UBTTaskNode_FindPatrolPos::UBTTaskNode_FindPatrolPos()
	: Radius(1000.f)
{
}

EBTNodeResult::Type UBTTaskNode_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Type = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	PrePosition = ControllingPawn->GetActorLocation();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld()); //컨트롤하는 Pawn이 위치한 월드를 가져옴

	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(PrePosition, Radius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPos"), NextPatrolPos.Location);
		return Type;
	}

	return EBTNodeResult::Failed;
}
