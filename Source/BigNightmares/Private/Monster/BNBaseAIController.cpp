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
	// AI 데이터 컴포넌트 초기화
	// ===============================
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// ===============================
	// AI 초기화: 블랙보드 + 비헤이비어 트리 실행
	// ===============================
	if (BlackboardData && UseBlackboard(BlackboardData, BlackboardComponent))
	{
		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
		}
	}

	
	// ===============================
	// AI 감지 시스템 연결
	// ===============================
	if (UAIPerceptionComponent* PC = GetPerceptionComponent())
	{
		PC->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// ===============================
	// 자극 감지 여부에 따라 타겟 설정/해제
	// ===============================
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
	// ===============================
	// 블랙보드에 타겟 액터 등록
	// ===============================
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(BBKeys::TargetActor, NewTarget);
	}
}

AActor* ABNBaseAIController::GetTargetActor() const
{
	// ===============================
	// 블랙보드에서 타겟 액터 반환
	// ===============================
	return BlackboardComponent ? Cast<AActor>(BlackboardComponent->GetValueAsObject(BBKeys::TargetActor)) : nullptr;
}

void ABNBaseAIController::ClearTargetActor()
{
	// ===============================
	// 블랙보드에서 타겟 정보 제거
	// ===============================
	if (BlackboardComponent)
	{
		BlackboardComponent->ClearValue(BBKeys::TargetActor);
	}
}