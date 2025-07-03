// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Monster/BNBlackboardKeys.h"

ABNBaseAIController::ABNBaseAIController()
{
	// ===============================
	// AI 데이터
	// ===============================
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// 감지 컴포넌트는 AAIController에 이미 존재하므로 별도 생성 없이 GetPerceptionComponent()로 접근
	// 바인딩은 OnPossess()에서 진행
}

void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 블랙보드 및 비헤이비어 트리 초기화
	if (BlackboardData && UseBlackboard(BlackboardData, BlackboardComponent))
	{
		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
		}
	}

	// 감지 이벤트 바인딩 (GetPerceptionComponent는 부모에서 제공됨)
	if (UAIPerceptionComponent* PC = GetPerceptionComponent())
	{
		PC->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		SetTargetActor(Actor);
	}
	else
	{
		ClearTargetActor();
	}
}

void ABNBaseAIController::SetTargetActor(AActor* NewTarget)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(BBKeys::TargetActor, NewTarget);
	}
}

AActor* ABNBaseAIController::GetTargetActor() const
{
	return BlackboardComponent ? Cast<AActor>(BlackboardComponent->GetValueAsObject(BBKeys::TargetActor)) : nullptr;
}

void ABNBaseAIController::ClearTargetActor()
{
	if (BlackboardComponent)
	{
		BlackboardComponent->ClearValue(BBKeys::TargetActor);
	}
}