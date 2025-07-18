// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()

public:
	// 생성자
	UBTService_CheckAttackRange();

protected:
	// 이 서비스가 활성화되어 있는 동안 주기적으로 호출
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 몬스터의 공격 가능 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 150.0f;
};
