// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteThinmanAttack.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBTTask_ExecuteThinmanAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteThinmanAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
