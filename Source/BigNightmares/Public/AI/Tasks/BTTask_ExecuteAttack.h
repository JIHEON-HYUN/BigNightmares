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
	UBTTask_ExecuteAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// [추가] TickTask 함수를 사용하겠다고 선언합니다.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
