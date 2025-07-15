// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "BNBaseMonster.generated.h"

class UAIPerceptionComponent;
class UBehaviorTree;
class UBNBaseAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNBaseMonster : public ABNBaseCharacter
{
	GENERATED_BODY()

public:
	ABNBaseMonster();

	// IAbilitySystemInterface 구현 함수
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AI 컨트롤러에 의해 빙의될 때 호출됩니다.
	virtual void PossessedBy(AController* NewController) override;

	// ... (ActivateMonster, HasStateTag 등 나머지 함수는 기존과 동일) ...
	// 이 몬스터가 사용할 행동 트리(Behavior Tree) 에셋입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	// 몬스터를 활성화시키는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void ActivateMonster();

	// 블루프린트에서 특정 상태 태그를 가지고 있는지 쉽게 확인할 수 있는 함수입니다.
	UFUNCTION(BlueprintPure, Category = "AI")
	bool HasStateTag(FGameplayTag StateTag) const;

	// --- 상태 전환 함수 ---
	UFUNCTION(BlueprintCallable, Category = "AI|State")
	virtual void EnterIdleState();

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	virtual void EnterChasingState();

	UFUNCTION(BlueprintCallable, Category = "AI|State")
	virtual void EnterAttackingState();

protected:
	// 게임이 시작될 때 또는 스폰될 때 호출됩니다.
	virtual void BeginPlay() override;

	// AI 로직 (행동 트리)을 시작하는 함수입니다.
	void StartAILogic();

	// GAS의 핵심 컴포넌트입니다. BNBaseMonster가 직접 소유합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBNBaseAbilitySystemComponent> AbilitySystemComponent;

	// AI Perception Component를 부모 클래스에 추가합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* PerceptionComponent;
	
	// 감지된 대상을 처리하는 함수입니다.
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	// 상태 전환을 처리하는 내부 헬퍼 함수입니다.
	void TransitionToState(const FGameplayTag& NewStateTag);
};