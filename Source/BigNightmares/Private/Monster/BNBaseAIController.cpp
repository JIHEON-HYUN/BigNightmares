// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"

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
		if (Monster->BehaviorTree && Monster->BehaviorTree->BlackboardAsset)
		{
			// [수정됨] UseBlackboard 함수는 두 번째 인자로 일반 포인터의 참조(UBlackboardComponent*&)를 요구합니다.
			// 따라서 TObjectPtr인 멤버 변수 'Blackboard'를 직접 전달할 수 없습니다.
			// 대신, 임시 일반 포인터 변수를 만들어 전달하여 함수 시그니처를 만족시킵니다.
			UBlackboardComponent* TempBlackboardComp = nullptr;
			if (UseBlackboard(Monster->BehaviorTree->BlackboardAsset, TempBlackboardComp))
			{
				// UseBlackboard 함수가 성공하면, 컨트롤러의 내장 Blackboard 멤버 변수가 자동으로 설정됩니다.
				// 이제 비헤이비어 트리를 시작할 수 있습니다.
				BehaviorTreeComponent->StartTree(*(Monster->BehaviorTree));
			}
		}
		
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
		}
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// Blackboard는 AAIController의 멤버 변수이므로 직접 접근할 수 있습니다.
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
