// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "Monster/BNHunterCharacter.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"
#include "DataAsset/DataAsset_State_Monster.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");

	// 이 태스크가 TickTask 함수를 호출하도록 설정합니다.
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ABNHunterCharacter* Hunter = Cast<ABNHunterCharacter>(AIController->GetPawn());
	if (!Hunter)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터의 공격 함수를 호출하여 몽타주 재생을 시작합니다.
	Hunter->EnterAttackingState();

	// 태스크가 즉시 끝나지 않고, TickTask에서 완료 여부를 결정하도록 InProgress를 반환합니다.
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ABNHunterCharacter* Hunter = Cast<ABNHunterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Hunter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const UAnimMontage* AttackMontage = Hunter->GetAttackMontage();
	UAnimInstance* AnimInstance = Hunter->GetMesh()->GetAnimInstance();

	if (!AttackMontage || !AnimInstance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 몽타주가 아직 재생 중인지 확인합니다.
	if (!AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		// [수정된 핵심 로직]
		// 몽타주가 끝났으므로, AI의 상태를 깨끗하게 초기화합니다.
		if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
		{
			// 1. 타겟 정보를 지워서, AI가 즉시 다시 추격하는 것을 막습니다.
			//    (시야 감지 시스템이 다음 프레임에 다시 타겟을 찾아줄 것입니다.)
			BlackboardComp->ClearValue(BBKeys::TargetActor);

			// 2. 상태를 'Idle'로 설정하여, 비헤이비어 트리가 Idle/Patrol 가지부터 다시 시작하도록 합니다.
			if (const UDataAsset_State_Monster* StateDataAsset = Hunter->StateDataAsset)
			{
				BlackboardComp->SetValueAsName(BBKeys::State, StateDataAsset->IdleStateTag.GetTagName());
			}
		}

		// 이 태스크가 성공적으로 완료되었음을 비헤이비어 트리에 알립니다.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
