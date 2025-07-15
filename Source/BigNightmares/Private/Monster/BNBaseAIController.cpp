// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"

ABNBaseAIController::ABNBaseAIController()
{
	// BlackboardComponent는 AAIController에 이미 내장되어 있으므로 생성하지 않습니다.
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABNBaseMonster* Monster = Cast<ABNBaseMonster>(InPawn))
	{
		// 몬스터가 사용할 비헤이비어 트리와 블랙보드를 설정합니다.
		if (Monster->BehaviorTree && Monster->BehaviorTree->BlackboardAsset)
		{
			// [수정됨] UseBlackboard 대신, 내장된 Blackboard 컴포넌트를 직접 초기화합니다.
			Blackboard->InitializeBlackboard(*(Monster->BehaviorTree->BlackboardAsset));
			BehaviorTreeComponent->StartTree(*(Monster->BehaviorTree));
		}
		
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
		}
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !Blackboard)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
	}
	else
	{
		Blackboard->ClearValue(TEXT("TargetActor"));
	}
}
