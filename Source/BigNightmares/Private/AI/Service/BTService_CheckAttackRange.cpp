// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
	// 서비스가 실행될 간격을 설정합니다. (예: 0.5초마다)
	Interval = 0.5f;
}

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	// 블랙보드에서 타겟 액터를 가져옵니다.
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKeys::TargetActor));
	if (!TargetActor)
	{
		return;
	}

	// 컨트롤러와 조종되는 몬스터를 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}
	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(AIController->GetPawn());
	if (!Monster)
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), TargetActor->GetActorLocation());
	const UDataAsset_State_Monster* StateData = Monster->StateDataAsset; // 데이터 에셋 가져오기

	if (DistanceToTarget <= AttackRange)
	{
		// 아직 공격 상태가 아닐 때만 상태를 변경합니다.
		if (!Monster->HasStateTag(StateData->AttackStateTag))
		{
			Monster->EnterAttackingState();
		}
	}
	else
	{
		// 아직 추격 상태가 아닐 때만 상태를 변경합니다.
		// (공격 중에 플레이어가 도망갔을 경우)
		if (!Monster->HasStateTag(StateData->ChaseStateTag))
		{
			Monster->EnterChasingState();
		}
	}
}
