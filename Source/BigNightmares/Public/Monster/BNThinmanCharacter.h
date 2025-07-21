// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "Interfaces/LightSensitive.h"
#include "GameplayTagContainer.h"
#include "BNThinmanCharacter.generated.h"

class UAnimMontage;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNThinmanCharacter : public ABNBaseMonster, public ILightSensitive
{
	GENERATED_BODY()

public:
	ABNThinmanCharacter();

	virtual void OnHitByLight_Implementation(bool bIsLit) override;
	virtual void EnterIdleState() override;
	virtual void EnterChasingState() override;
	virtual void EnterAttackingState() override;

protected:
	// [해결책] AI 컨트롤러가 빙의될 때 호출되는 함수를 오버라이드합니다.
	virtual void PossessedBy(AController* NewController) override;

	// [해결책] 타이머가 호출할 함수입니다.
	void SetInitialStateToIdle();

	// [해결책] 타이머를 관리하기 위한 핸들입니다.
	FTimerHandle InitialStateTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|State")
	FGameplayTag StunnedByLightTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 100.0f;
};
