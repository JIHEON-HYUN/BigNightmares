// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteAttack.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBTTask_ExecuteAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// 생성자
	UBTTask_ExecuteAttack();

protected:
	// BehaviorTree에 의해 실행될 때 최초로 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 애니메이션 재생이 완료되었는지 확인
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
