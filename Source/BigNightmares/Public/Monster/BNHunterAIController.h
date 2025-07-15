// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BNBaseAIController.h"
#include "BNHunterAIController.generated.h"

class UAISenseConfig_Sight;
class AActor;
class UBehaviorTree;

class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNHunterAIController : public ABNBaseAIController
{
	GENERATED_BODY()

public:
	ABNHunterAIController();

protected:
	// 헌터의 시각 감지 설정을 위한 변수입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
