// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BNBaseAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BNThinmanAIController.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTree; // [추가]

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNThinmanAIController : public ABNBaseAIController
{
	GENERATED_BODY()

public:
	ABNThinmanAIController();

protected:
	// 씬맨의 시각 감지 설정을 위한 변수입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
