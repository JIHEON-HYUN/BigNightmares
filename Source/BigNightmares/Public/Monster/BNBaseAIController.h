// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BNBaseAIController.generated.h"

class UAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNBaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABNBaseAIController();

protected:
	virtual void BeginPlay() override;

	// ===========================================
	// AI 지각 시스템 (AI Perception System)
	// ===========================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
