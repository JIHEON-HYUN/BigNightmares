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

	// ================================
	// 감지 시스템 구성
	// ================================

	// 시야 감지 설정
	UPROPERTY(Visibleanywhere, BlueprintReadOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;

	// 감지 이벤트 (OnPerceptionUpdated)
	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);
	
	
	// ================================
	// 추적 대상 및 상태
	// ================================
	
	// 현재 추적 대상
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	AActor* Target;

	// 대상 추적 중 여부
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bIsTracking = false;
	

	// ================================
	// 이동 및 전투 설정
	// ================================
	
	// 기본 걷기 속도
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Movement")
	float WalkSpeed = 200.f;

	// 추격 시 이동 속도
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Movement")
	float ChaseSpeed = 400.f;

	// 공격 가능 범위
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float AttackRange = 150.f;


	// ================================
	// 상태별 AI 동작 처리
	// ================================
	
	// 상태별 로직 처리
	void HandleIdle();
	void HandleChase();
	void HandleAttack();
};
