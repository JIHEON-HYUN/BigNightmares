// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindPatrolPos.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	// 태스크 노드의 이름을 설정합니다. 에디터에서 보일 이름입니다.
	NodeName = TEXT("Find Patrol Position");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러와 컨트롤러가 조종하는 폰(캐릭터)을 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 현재 월드의 내비게이션 시스템을 가져옵니다.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터의 현재 위치를 중심으로 PatrolRadius 반경 내에서 임의의 지점을 찾습니다.
	FNavLocation RandomLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(ControlledPawn->GetActorLocation(), PatrolRadius, RandomLocation))
	{
		// 성공적으로 위치를 찾았다면, 블랙보드의 'PatrolLocation' 키에 해당 위치 벡터를 저장합니다.
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKeys::PatrolLocation, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	// 위치 찾기에 실패하면 태스크를 실패 처리합니다.
	return EBTNodeResult::Failed;
}
