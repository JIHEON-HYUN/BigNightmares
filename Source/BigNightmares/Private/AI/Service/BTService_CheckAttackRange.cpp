// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BNBaseMonster.h"
#include "Player/BNMonoCharacter.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"
#include "Kismet/GameplayStatics.h"

// 생성자
UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
	Interval = 0.5f;
}

// TickNode
void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Monster || !Monster->StateDataAsset) return;

	if (Monster->HasStateTag(Monster->StateDataAsset->AttackStateTag))
	{
		return;
	}

	// --- [핵심 수정] 가장 가까운 플레이어를 찾아 타겟으로 설정하는 로직 ---

	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABNMonoCharacter::StaticClass(), FoundPlayers);

	if (FoundPlayers.Num() == 0)
	{
		return;
	}

	AActor* ClosestPlayer = nullptr;
	float MinDistance = AttackRange; // 최대 사정거리로 초기화

	// 모든 플레이어와의 거리를 확인하여 가장 가까운 플레이어를 찾습니다.
	for (AActor* PlayerActor : FoundPlayers)
	{
		if (PlayerActor)
		{
			float Distance = Monster->GetDistanceTo(PlayerActor);
			if (Distance <= MinDistance)
			{
				MinDistance = Distance;
				ClosestPlayer = PlayerActor;
			}
		}
	}

	// 사정거리 내에 가장 가까운 플레이어를 찾았다면
	if (ClosestPlayer)
	{
		// Blackboard의 Target과 State를 모두 업데이트합니다.
		BlackboardComp->SetValueAsObject(BBKeys::TargetActor, ClosestPlayer);
		BlackboardComp->SetValueAsName(BBKeys::State, Monster->StateDataAsset->AttackStateTag.GetTagName());
	}
}