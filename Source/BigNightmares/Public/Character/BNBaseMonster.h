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
	ABNBaseMonster();

	// IAbilitySystemInterface 구현 함수
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 이 몬스터가 사용할 비헤이비어 트리 에셋입니다. AI 컨트롤러가 이 정보를 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// 블루프린트에서 각 몬스터 종류에 맞는 데이터 에셋을 할당해줍니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|State")
	TObjectPtr<UDataAsset_State_Monster> StateDataAsset;

protected:
	// AI 컨트롤러에 의해 빙의될 때 호출됩니다.
	virtual void PossessedBy(AController* NewController) override;

	// 게임이 시작될 때 또는 스폰될 때 호출됩니다.
	virtual void BeginPlay() override;

	// GAS의 핵심 컴포넌트입니다. BNBaseMonster가 직접 소유합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBNBaseAbilitySystemComponent> AbilitySystemComponent;

	// [추가된 부분]
	// 애니메이션 블루프린트에서 현재 상태를 알 수 있도록 이 변수를 추가합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	FGameplayTag CurrentStateTag;

public:
	virtual void ActivateMonster();
	bool HasStateTag(FGameplayTag StateTag) const;
	virtual void EnterIdleState();
	virtual void EnterChasingState();
	virtual void EnterAttackingState();

private:
	// AI 관련 로직이 모두 AIController로 이전되었으므로, 관련 함수 선언을 모두 삭제합니다.
	void TransitionToState(const FGameplayTag& NewStateTag);
};