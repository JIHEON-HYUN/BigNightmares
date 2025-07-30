// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_CheckAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BNBaseMonster.h"
#include "Player/BNMonoCharacter.h" // [수정] BNBaseCharacter -> BNMonoCharacter
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"
#include "Kismet/GameplayStatics.h"

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
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Monster || !Monster->StateDataAsset) return;

	if (Monster->HasStateTag(Monster->StateDataAsset->AttackStateTag))
	{
		return;
	}

	// --- 여기가 핵심 수정 부분입니다 ---

	// 1. 월드에 있는 모든 플레이어 액터를 가져옵니다.
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABNMonoCharacter::StaticClass(), FoundPlayers);

	if (FoundPlayers.Num() == 0)
	{
		return; // 플레이어가 없으면 검사 종료
	}
	
	// 2. 모든 플레이어와의 거리를 확인합니다.
	for (AActor* PlayerActor : FoundPlayers)
	{
		if (PlayerActor)
		{
			// 3. 거리가 이 서비스의 멤버 변수인 AttackRange 안에 있다면,
			if (Monster->GetDistanceTo(PlayerActor) <= AttackRange)
			{
				// Blackboard의 State를 Attack으로 변경하고 즉시 종료합니다.
				BlackboardComp->SetValueAsName(BBKeys::State, Monster->StateDataAsset->AttackStateTag.GetTagName());
				return;
			}
		}
	}
}
