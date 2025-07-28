// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ExecuteThinmanAttack.h"
#include "Monster/BNThinmanCharacter.h" // [수정] 씬맨 캐릭터로 변경
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "AIController.h"

UBTTask_ExecuteThinmanAttack::UBTTask_ExecuteThinmanAttack()
{
	NodeName = TEXT("Execute Thinman Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ExecuteThinmanAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABNThinmanCharacter* Thinman = Cast<ABNThinmanCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Thinman) return EBTNodeResult::Failed;

	Thinman->EnterAttackingState(); // 씬맨의 공격 몽타주 재생
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteThinmanAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABNThinmanCharacter* Thinman = Cast<ABNThinmanCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Thinman)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// [수정] 변수에 직접 접근하는 대신, public 함수를 통해 가져옵니다.
	const UAnimMontage* AttackMontage = Thinman->GetAttackMontage();
	UAnimInstance* AnimInstance = Thinman->GetMesh()->GetAnimInstance();

	if (!AttackMontage || !AnimInstance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 몽타주가 끝났다면 상태를 초기화합니다.
	if (!AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
		{
			BlackboardComp->ClearValue(BBKeys::TargetActor);
			if (const UDataAsset_State_Monster* StateDataAsset = Thinman->StateDataAsset)
			{
				BlackboardComp->SetValueAsName(BBKeys::State, StateDataAsset->IdleStateTag.GetTagName());
			}
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
