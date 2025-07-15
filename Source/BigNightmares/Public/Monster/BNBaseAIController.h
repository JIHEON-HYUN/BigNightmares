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
	// AI 컨트롤러가 폰(캐릭터)에 빙의될 때 호출됩니다.
	virtual void OnPossess(APawn* InPawn) override;

	// [수정됨] 자식 클래스에서 접근하고 재정의할 수 있도록 protected / virtual로 변경합니다.
protected:
	// AI의 행동 로직을 담당하는 비헤이비어 트리 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	// AI의 감각(시각, 청각 등)을 담당하는 감지 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Components")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	// 감지된 대상을 처리하는 함수입니다.
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
