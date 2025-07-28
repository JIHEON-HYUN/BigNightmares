// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "Monster/BNHunterCharacter.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"
#include "DataAsset/DataAsset_State_Monster.h"

// 생성자
UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	// BehaviorTree에 저장될 이름
	NodeName = TEXT("Execute Attack");
	
	// 이 태스크가 TickTask 함수를 호출하도록 설정
	bNotifyTick = true;
}

// 태스크 시작 시 호출되는 함수
EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 클래스 호출
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AIController를 가져와서 저장
	AAIController* AIController = OwnerComp.GetAIOwner();
	// 예외 처리 코드
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// AIController가 조종하는 몬스터를 Cast
	ABNHunterCharacter* Hunter = Cast<ABNHunterCharacter>(AIController->GetPawn());
	// 예외 처리
	if (!Hunter)
	{
		return EBTNodeResult::Failed;
	}

	// 캐릭터의 공격 함수를 호출하여 몽타주 재생을
	Hunter->EnterAttackingState();

	// 태스크가 즉시 끝나지 않고, TickTask에서 완료 여부를 결정하도록 InProgress를 반환
	return EBTNodeResult::InProgress;
}

// ExecuteTask가 InProgress를 반환할 때 호출되는 함수
void UBTTask_ExecuteAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 부모 클래스 호출
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// AI가 조종하는 몬스터를 Cast
	ABNHunterCharacter* Hunter = Cast<ABNHunterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	// 예외 처리 함수
	if (!Hunter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 캐릭터로부터 현재 재생해야 할 공격 몽타주와 애니메이션 인스턴스를 가져옴
	const UAnimMontage* AttackMontage = Hunter->GetAttackMontage();
	UAnimInstance* AnimInstance = Hunter->GetMesh()->GetAnimInstance();

	// 예외 처리 코드
	if (!AttackMontage || !AnimInstance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 몽타주가 아직 재생 중인지 확인
	if (!AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		// 몽타주가 끝나면 AI의 상태를 초기화
		if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
		{
			// 타겟 정보를 지워 AI가 즉시 다시 추격하는 것을 방지
			BlackboardComp->ClearValue(BBKeys::TargetActor);

			// 상태를 'Idle'로 설정 BehaviorTree가 Idle/Patrol 가지부터 다시 시작하도록 설정
			if (const UDataAsset_State_Monster* StateDataAsset = Hunter->StateDataAsset)
			{
				BlackboardComp->SetValueAsName(BBKeys::State, StateDataAsset->IdleStateTag.GetTagName());
			}
		}

		// 태스크가 성공적으로 완료되었음을 비헤이비어 트리에 알림
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
