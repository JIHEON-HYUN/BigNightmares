// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "BNHunterCharacter.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNHunterCharacter : public ABNBaseMonster
{
	GENERATED_BODY()

public:
	ABNHunterCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 시야 설정
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	// 추적 대상
	UPROPERTY()
	AActor* Target;

	// 추적 여부
	UPROPERTY(BlueprintReadOnly, Category = "MonsterAI")
	bool bIsTracking = false;

	// 추격 시 이동 속도
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "MonsterAI")
	float ChaseSpeed = 400.f;

	// 기본 속도
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "MonsterAI")
	float WalkSpeed = 200.f;

	// 공격 범위
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "MonsterAI")
	float AttackRange = 150.f;

	// 감지 시 호출
	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

	// 상태별 로직 처리
	void HandleIdle();
	void HandleChase();
	void HandleAttack();
};
