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

	// ====================
	// 외부 인터페이스
	// ====================
	
	// 현재 몬스터 상태 반환
	UFUNCTION(BlueprintCallable, Category = "State")
	EMonsterState GetMonsterState() const;

	// 몬스터 활성화 (미션 클리어 개수에 따라 활성화)
	UFUNCTION(BlueprintCallable, Category = "State")
	void ActivateMonster();
	

protected:
	virtual void BeginPlay() override;

	// ====================
	// 상태 변수 및 제어
	// ====================
	
	// 현재 몬스터 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EMonsterState CurrentState;

	// 몬스터 활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActive = false;

	// 몬스터 상태 변경
	void SetMonsterState(EMonsterState NewState);

};
