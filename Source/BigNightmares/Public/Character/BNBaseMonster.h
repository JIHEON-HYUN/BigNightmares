// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "BNBaseMonster.generated.h"

class UBehaviorTree;
class UBNBaseAbilitySystemComponent;
class UDataAsset_State_Monster;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNBaseMonster : public ABNBaseCharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ABNBaseMonster();

	// 어빌리티 시스템 컴포넌트 Getter
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AI가 사용할 비헤이비어 트리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// 몬스터 상태 데이터 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|State")
	TObjectPtr<UDataAsset_State_Monster> StateDataAsset;

protected:
	// AI 컨트롤러 빙의 시 호출
	virtual void PossessedBy(AController* NewController) override;

	// 게임 시작 또는 스폰 시 호출
	virtual void BeginPlay() override;

	// 게임플레이 어빌리티 시스템 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBNBaseAbilitySystemComponent> AbilitySystemComponent;

	// 현재 상태 태그 (애니메이션용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FGameplayTag CurrentStateTag;

public:
	// 몬스터 활성화 함수
	virtual void ActivateMonster();
	// 특정 상태 태그 보유 여부 확인 함수
	bool HasStateTag(FGameplayTag StateTag) const;
	// 대기 상태 진입 함수
	virtual void EnterIdleState();
	// 추격 상태 진입 함수
	virtual void EnterChasingState();
	// 공격 상태 진입 함수
	virtual void EnterAttackingState();

private:
	// 상태 변경 처리 함수
	void TransitionToState(const FGameplayTag& NewStateTag);
};