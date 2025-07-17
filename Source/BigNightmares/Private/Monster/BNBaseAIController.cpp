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
		// 몬스터가 유효한 비헤이비어 트리를 가지고 있다면 실행합니다.
		if (Monster->BehaviorTree)
		{
			RunBehaviorTree(Monster->BehaviorTree);
			
		}
		
		// AIPerceptionComponent의 델리게이트를 바인딩합니다.
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
		UE_LOG(LogTemp, Log, TEXT("AIController's Blackboard State was set to: %s (Called by Monster)"), *InitialState.ToString());
	}
	else
	{
		// 이 로그는 보통 OnPossess가 아직 호출되지 않았을 때 나타날 수 있습니다.
		UE_LOG(LogTemp, Warning, TEXT("SetInitialStateOnBlackboard was called, but BlackboardComponent is not yet valid."));
	}
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// GetBlackboardComponent()를 직접 사용하도록 수정하여 Blackboard 멤버 변수에 대한 의존성을 제거합니다.
	if (!Actor || !GetBlackboardComponent())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// [개선] 문자열 대신 정의된 키를 사용하여 안전하게 값을 설정합니다.
		GetBlackboardComponent()->SetValueAsObject(BBKeys::TargetActor, Actor);
	}
	else
	{
		// [개선] 문자열 대신 정의된 키를 사용하여 안전하게 값을 초기화합니다.
		GetBlackboardComponent()->ClearValue(BBKeys::TargetActor);
	}
}
