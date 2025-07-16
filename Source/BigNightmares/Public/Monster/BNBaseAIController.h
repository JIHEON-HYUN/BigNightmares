// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BNBaseAIController.generated.h"

class UBehaviorTreeComponent;
class UAIPerceptionComponent;
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

	// [추가] 몬스터가 자신의 초기 상태를 블랙보드에 설정하기 위해 호출할 함수입니다.
	void SetInitialStateOnBlackboard(FName InitialState);
	
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
