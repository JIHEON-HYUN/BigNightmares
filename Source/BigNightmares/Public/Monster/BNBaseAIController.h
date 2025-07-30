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
	// 생성자
	ABNBaseAIController();

	// 블랙보드 초기 상태 설정 함수
	void SetInitialStateOnBlackboard(FName InitialState);
	
protected:
	// 폰(Pawn)에 빙의했을 때 호출되는 함수
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	// 비헤이비어 트리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	// AI가 사용할 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// AI 인지(시야, 청각 등) 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	// 인지 시스템이 업데이트될 때(대상을 감지하거나 잃을 때) 호출될 함수
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
