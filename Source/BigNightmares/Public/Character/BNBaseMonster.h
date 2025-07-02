// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseCharacter.h"
#include "BNBaseMonster.generated.h"

UENUM(Blueprintable)
enum class EMonsterState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Idle		UMETA(DisplayName = "Idle"),
	Chase		UMETA(DisplayName = "Chase"),
	Attack		UMETA(DisplayName = "Attack")
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNBaseMonster : public ABNBaseCharacter
{
	GENERATED_BODY()

public:
	ABNBaseMonster();

protected:
	virtual void BeginPlay() override;

	// 현재 몬스터 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	EMonsterState CurrentState;

	// 몬스터 활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	bool bIsActive = false;

protected:
	// 몬스터 상태 변경
	void SetMonsterState(EMonsterState NewState);

public:
	// 현재 상태 반환
	UFUNCTION(BlueprintCallable, Category = "Monster")
	EMonsterState GetMonsterState() const { return CurrentState; }

	// 미션 성공시 몬스터 활성화
	UFUNCTION(BlueprintCallable, Category = "Monster")
	void ActivateMonster();
};
