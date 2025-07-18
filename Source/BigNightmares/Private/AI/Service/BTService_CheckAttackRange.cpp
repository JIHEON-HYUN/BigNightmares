// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"

// 생성자
UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	// BehaviorTree에 저장될 이름
	NodeName = TEXT("Check Attack Range");
	// TickNode 호출 주기
	Interval = 0.5f;
}

// TickNode
void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 부모 클래스 호출
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// BehaviorTree를 소유한 AI의 BlackboardComponent를 저장
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	// 예외 처리 코드
	if (!BlackboardComp)
	{
		return;
	}

	// Blackboard에서 'TargetActor' 키에 저장된 값을 Cast
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BBKeys::TargetActor));
	// 예외 처리 코드
	if (!TargetActor)
	{
		return;
	}

	// AIController를 통해 현재 조종하고 있는 몬스터를 Cast
	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	// 예외 처리 코드
	if (!Monster || !Monster->StateDataAsset)
	{
		return;
	}

	// 몬스터가 이미 AttackingStateTag를 가지고 있는지 확인 (Tag 보유시 함수 종료)
	if (Monster->HasStateTag(Monster->StateDataAsset->AttackStateTag))
	{
		return;
	}

	// 몬스터의 현재 위치와 타겟의 현재 위치 사이의 거리를 계산
	const float DistanceToTarget = FVector::Dist(Monster->GetActorLocation(), TargetActor->GetActorLocation());

	// DistanceToTarget의 거리가 AttackRange보다 작거나 같다면 실행
	if (DistanceToTarget <= AttackRange)
	{
		// Blackboard의 State 키 값을 AttackStateTag로 변경
		BlackboardComp->SetValueAsName(BBKeys::State, Monster->StateDataAsset->AttackStateTag.GetTagName());
	}
}
