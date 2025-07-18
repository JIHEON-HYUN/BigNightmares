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

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKeys::TargetActor));
	if (!TargetActor)
	{
		return;
	}

	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Monster || !Monster->StateDataAsset)
	{
		return;
	}

	if (Monster->HasStateTag(Monster->StateDataAsset->AttackStateTag))
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), TargetActor->GetActorLocation());

	if (DistanceToTarget <= AttackRange)
	{
		// 이 서비스의 유일한 역할은 블랙보드의 상태 값을 변경하여,
		// 비헤이비어 트리가 'Attack' 가지로 넘어가도록 신호를 보내는 것입니다.
		BlackboardComp->SetValueAsName(BBKeys::State, Monster->StateDataAsset->AttackStateTag.GetTagName());
	}
}
