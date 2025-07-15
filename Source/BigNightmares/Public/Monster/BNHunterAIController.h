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
	ABNHunterAIController();

	// [신규] 블랙보드에서 사용할 키의 이름을 반환하는 함수
	static const FName TargetPlayerKey;

protected:
	virtual void BeginPlay() override;
	
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Sight")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
};
