// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindRandomPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBTT_FindRandomPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FindRandomPatrolLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 순찰 반경
	UPROPERTY(EditAnywhere, Category = "AI")
	float Radius = 1000.0f;

	// 순찰 위치를 저장할 블랙보드 키
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector PatrolLocation;
};
