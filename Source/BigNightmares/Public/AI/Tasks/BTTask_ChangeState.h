// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeState.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idle,
	Chasing,
	Attacking
};
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBTTask_ChangeState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// BT 에디터에서 어떤 상태로 변경할지 선택할 수 있는 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EMonsterState StateToChange;
	
};
