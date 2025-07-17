// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BNBaseMonster.h"
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

	// 몬스터와 타겟 사이의 거리를 계산합니다.
	const float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), TargetActor->GetActorLocation());

	if (DistanceToTarget <= AttackRange)
	{
		// 거리가 공격 범위 이내라면, 공격 상태로 전환합니다.
		Monster->EnterAttackingState();
	}
	else
	{
		// 거리가 공격 범위 밖이라면, 다시 추격 상태로 돌아갑니다.
		// (공격 중에 플레이어가 도망갔을 경우를 대비한 로직)
		Monster->EnterChasingState();
	}
}
