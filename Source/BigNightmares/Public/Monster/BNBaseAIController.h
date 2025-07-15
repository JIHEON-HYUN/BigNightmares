// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BNBaseAIController.generated.h"

class UBehaviorTreeComponent;
class UAIPerceptionComponent;
// [수정됨] FAIStimulus는 class가 아닌 struct입니다.
struct FAIStimulus;

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
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
