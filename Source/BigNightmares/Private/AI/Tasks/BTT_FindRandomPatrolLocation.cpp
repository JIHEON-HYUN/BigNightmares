// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTT_FindRandomPatrolLocation.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindRandomPatrolLocation::UBTT_FindRandomPatrolLocation()
{
	NodeName = TEXT("Find Random Patrol Location");
}

EBTNodeResult::Type UBTT_FindRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러와 현재 위치 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	FVector CurrentLocation = AIPawn->GetActorLocation();

	// 네비게이션 시스템을 통해 랜덤 위치 찾기
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;
	bool bFound = NavSystem->GetRandomReachablePointInRadius(CurrentLocation, Radius, RandomLocation);

	if (bFound)
	{
		// 찾은 위치를 블랙보드에 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocation.SelectedKeyName, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}