// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/BNBaseMonster.h"
#include "Monster/BNMonsterGameplayTags.h"

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
			// 1. 블랙보드를 설정합니다.
			UBlackboardComponent* TempBlackboardComp = nullptr;
			if (UseBlackboard(Monster->BehaviorTree->BlackboardAsset, TempBlackboardComp))
			{
				// 2. [핵심 수정] 몬스터의 현재 상태(GameplayTag)를 읽어서 블랙보드의 초기값을 설정합니다.
				// OnPossess는 BeginPlay 이후에 호출되므로, 몬스터는 이미 Dormant 태그를 가지고 있습니다.
				const FBNMonsterGameplayTags& GameplayTags = FBNMonsterGameplayTags::Get();
				FName InitialStateName = NAME_None;

				if (Monster->HasStateTag(GameplayTags.Character_Monster_Dormant))
				{
					InitialStateName = GameplayTags.Character_Monster_Dormant.GetTagName();
					UE_LOG(LogTemp, Log, TEXT("AIController possessed a Dormant monster. Setting Blackboard State to: %s"), *InitialStateName.ToString());
				}
				
				Blackboard->SetValueAsName(TEXT("State"), InitialStateName);

				// 3. 이제 블랙보드가 올바른 초기 상태를 알게 되었으므로, 비헤이비어 트리를 시작합니다.
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
