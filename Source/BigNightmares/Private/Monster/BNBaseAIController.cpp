// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"

ABNBaseAIController::ABNBaseAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ABNBaseMonster* Monster = Cast<ABNBaseMonster>(InPawn))
	{
		if (Monster->BehaviorTree)
		{
			RunBehaviorTree(Monster->BehaviorTree);
		}
		
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
		}
	}
}

void ABNBaseAIController::SetInitialStateOnBlackboard(FName InitialState)
{
	if (UBlackboardComponent* MyBlackboard = GetBlackboardComponent())
	{
		MyBlackboard->SetValueAsName(BBKeys::State, InitialState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetInitialStateOnBlackboard was called, but BlackboardComponent is not yet valid."));
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* MyBlackboard = GetBlackboardComponent();
	if (!Actor || !MyBlackboard)
	{
		return;
	}

	ABNBaseMonster* Monster = Cast<ABNBaseMonster>(GetPawn());
	if (!Monster || !Monster->StateDataAsset)
	{
		return;
	}

    // [수정된 핵심 로직]
    // AI의 현재 상태를 블랙보드에서 직접 확인합니다.
    const FName CurrentStateName = MyBlackboard->GetValueAsName(BBKeys::State);
    const FName AttackStateName = Monster->StateDataAsset->AttackStateTag.GetTagName();

    // 만약 현재 상태가 'Attacking'이라면, 시야 감지 업데이트를 무시합니다.
    // 이렇게 하면 공격 애니메이션이 끝날 때까지 다른 상태로 변경되지 않습니다.
    if (CurrentStateName == AttackStateName)
    {
        return; // 공격을 방해하지 않고 함수를 즉시 종료합니다.
    }

	// 휴면 상태일 때도 무시합니다. (기존 로직 유지)
	const FName DormantStateName = Monster->StateDataAsset->DormantStateTag.GetTagName();
	if (CurrentStateName == DormantStateName)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		MyBlackboard->SetValueAsObject(BBKeys::TargetActor, Actor);
		Monster->EnterChasingState();
	}
	else
	{
		MyBlackboard->ClearValue(BBKeys::TargetActor);
		Monster->EnterIdleState();
	}
}
