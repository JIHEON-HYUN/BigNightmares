// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindPatrolPos.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"

// 생성자
UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	// BehaviorTree에 저장될 이름
	NodeName = TEXT("Find Patrol Position");
}

// 태스크가 실행될 때 호출되는 함수
EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 클래스 호출
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIController를 가져옴
	AAIController* AIController = OwnerComp.GetAIOwner();
	// 예외 처리 코드
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	// AIController가 조종하는 몬스터를 가져옴
	APawn* ControlledPawn = AIController->GetPawn();
	// 예외 처리 코드
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 현재 월드의 내비게이션 시스템을 가져옴
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	// 예외 처리 코드
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	// 내비게이션 시스템이 길을 찾을 수 있는 반경 내에서 랜덤한 위치를 저장
	FNavLocation RandomLocation;
	// 랜덤한 위치를 찾음 (성공시 true를 반환)
	if (NavSystem->GetRandomPointInNavigableRadius(ControlledPawn->GetActorLocation(), PatrolRadius, RandomLocation))
	{
		// 성공적으로 위치를 찾았다면, 블랙보드의 'PatrolLocation' 키에 해당 위치 벡터를 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKeys::PatrolLocation, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	// 위치 찾기에 실패하면 태스크를 실패 처리
	return EBTNodeResult::Failed;
}
