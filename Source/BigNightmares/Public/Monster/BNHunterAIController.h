// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BNBaseAIController.h"
#include "BNHunterAIController.generated.h"

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
	virtual void SetupPerception() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
