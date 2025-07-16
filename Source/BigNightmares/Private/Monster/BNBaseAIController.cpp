// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"
#include "DataAsset/DataAsset_State_Monster.h"

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
			
			// [핵심 수정] OnPossess 시점에 몬스터의 상태를 확인하고 블랙보드를 설정합니다.
			// 이 시점에는 BeginPlay가 이미 호출되었을 수도, 아닐 수도 있습니다.
			if (Monster->StateDataAsset)
			{
				const FGameplayTag DormantTag = Monster->StateDataAsset->DormantStateTag;
				if (Monster->HasStateTag(DormantTag))
				{
					SetInitialStateOnBlackboard(DormantTag.GetTagName());
				}
			}
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
		MyBlackboard->SetValueAsName(TEXT("State"), InitialState);
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
