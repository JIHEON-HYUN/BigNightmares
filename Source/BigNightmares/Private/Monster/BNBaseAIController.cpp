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
	// 비헤이비어 트리 컴포넌트 생성
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	// AI 인지 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

// 폰(Pawn)에 빙의했을 때 호출
void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 빙의한 폰을 몬스터로 Cast
	if (ABNBaseMonster* Monster = Cast<ABNBaseMonster>(InPawn))
	{
		// 몬스터의 비헤이비어 트리 실행
		if (Monster->BehaviorTree)
		{
			RunBehaviorTree(Monster->BehaviorTree);
		}

		// 인지 업데이트 이벤트에 함수 바인딩
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
		}
	}
}

// 블랙보드 초기 상태 설정
void ABNBaseAIController::SetInitialStateOnBlackboard(FName InitialState)
{
	// 블랙보드 컴포넌트 유효성 확인
	if (UBlackboardComponent* MyBlackboard = GetBlackboardComponent())
	{
		// State 키에 초기 상태 값 설정
		MyBlackboard->SetValueAsName(BBKeys::State, InitialState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetInitialStateOnBlackboard was called, but BlackboardComponent is not yet valid."));
	}
}

// 인지 시스템 업데이트 시 호출
void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 유효성 확인
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

	// 블랙보드에서 현재 상태 값 가져오기
    const FName CurrentStateName = MyBlackboard->GetValueAsName(BBKeys::State);
    const FName AttackStateName = Monster->StateDataAsset->AttackStateTag.GetTagName();

	// 현재 공격 중이라면 인지 업데이트 무시 (공격 방해 방지)
    if (CurrentStateName == AttackStateName)
    {
	    return;
    }

	// 현재 휴면 상태라면 인지 업데이트 무시
	const FName DormantStateName = Monster->StateDataAsset->DormantStateTag.GetTagName();
	if (CurrentStateName == DormantStateName)
	{
		return;
	}

	// 대상을 성공적으로 감지했다면
	if (Stimulus.WasSuccessfullySensed())
	{
		// 블랙보드에 타겟 설정
		MyBlackboard->SetValueAsObject(BBKeys::TargetActor, Actor);
		// 추격 상태로 전환
		Monster->EnterChasingState();
	}
	// 대상을 잃었다면
	else
	{
		// 블랙보드에서 타겟 제거
		MyBlackboard->ClearValue(BBKeys::TargetActor);
		// 대기 상태로 전환
		Monster->EnterIdleState();
	}
}
