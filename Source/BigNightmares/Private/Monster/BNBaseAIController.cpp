// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBlackboardKeys.h"

// 생성자
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
		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
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

	const FName CurrentStateName = MyBlackboard->GetValueAsName(BBKeys::State);
    const FName AttackStateName = Monster->StateDataAsset->AttackStateTag.GetTagName();

	// 현재 공격 중이라면 인지 업데이트 무시
    if (CurrentStateName == AttackStateName)
    {
	    return;
    }

	// [핵심 수정] 대상을 성공적으로 감지했다면, 휴면 상태라도 즉시 깨어나서 추격합니다.
	if (Stimulus.WasSuccessfullySensed())
	{
		MyBlackboard->SetValueAsObject(BBKeys::TargetActor, Actor);
		Monster->EnterChasingState();
	}
	// 대상을 잃었다면
	else
	{
		// [핵심 수정] 현재 휴면 상태라면, 타겟을 잃어도 아무것도 하지 않고 계속 잠들어 있습니다.
		// 이렇게 하면 활성화 트리거를 기다리는 몬스터가 다시 잠드는 것을 방지합니다.
		const FName DormantStateName = Monster->StateDataAsset->DormantStateTag.GetTagName();
		if (CurrentStateName == DormantStateName)
		{
			return;
		}

		MyBlackboard->ClearValue(BBKeys::TargetActor);
		Monster->EnterIdleState();
	}
}
