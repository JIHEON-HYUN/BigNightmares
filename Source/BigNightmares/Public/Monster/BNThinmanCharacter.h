// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "Interfaces/LightSensitive.h"
#include "BNThinmanCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNThinmanCharacter : public ABNBaseMonster, public ILightSensitive
{
	GENERATED_BODY()

public:
	ABNThinmanCharacter();

	// ILightSensitive 인터페이스의 함수를 실제로 구현하겠다고 선언합니다.
	virtual void OnHitByLight_Implementation(bool bIsLit) override;

	// 상태 변경 함수들을 오버라이드합니다.
	virtual void EnterIdleState() override;
	virtual void EnterChasingState() override;

protected:
	// 게임 시작 또는 스폰 시 호출됩니다.
	virtual void BeginPlay() override;

	// 몬스터가 빛에 의해 멈췄을 때를 나타내는 게임플레이 태그입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|State")
	FGameplayTag StunnedByLightTag;

	// 평소 걷기 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 50.0f;

	// 플레이어 추격 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 80.0f;
};
