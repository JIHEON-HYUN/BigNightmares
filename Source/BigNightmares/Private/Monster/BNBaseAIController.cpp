// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h" // [오류 수정] 이 헤더 파일이 반드시 포함되어야 합니다.
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

	// [수정] 블랙보드에서 현재 상태 값을 직접 가져와서 확인합니다.
	const FName CurrentStateName = MyBlackboard->GetValueAsName(BBKeys::State);
	const FName DormantStateName = Monster->StateDataAsset->DormantStateTag.GetTagName();

	if (CurrentStateName == DormantStateName)
	{
		// 현재 상태가 Dormant라면, 아무것도 하지 않고 즉시 함수를 종료합니다.
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
