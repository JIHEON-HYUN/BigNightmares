// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ChangeState.h"
#include "AIController.h"
#include "Character/BNBaseMonster.h"

UBTTask_ChangeState::UBTTask_ChangeState()
{
	NodeName = TEXT("Change Monster State");
}

EBTNodeResult::Type UBTTask_ChangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러를 통해 현재 조종 중인 몬스터를 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(AIController->GetPawn());
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	// 에디터에서 선택한 상태에 따라 몬스터의 상태 변경 함수를 호출합니다.
	switch (StateToChange)
	{
	case EMonsterState::Idle:
		Monster->EnterIdleState();
		break;
	case EMonsterState::Chasing:
		Monster->EnterChasingState();
		break;
	case EMonsterState::Attacking:
		Monster->EnterAttackingState();
		break;
	default:
		// 유효하지 않은 상태일 경우 실패 처리합니다.
		return EBTNodeResult::Failed;
	}

	// 상태 변경에 성공했으므로 태스크를 성공 처리합니다.
	return EBTNodeResult::Succeeded;
}
