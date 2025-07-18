// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BNBaseAIController.h"
#include "BNHunterAIController.generated.h"

class UAISenseConfig_Sight;
class AActor;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNHunterAIController : public ABNBaseAIController
{
	GENERATED_BODY()

public:
	// 생성자
	ABNHunterAIController();

protected:
	// 시야 감지 설정 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
