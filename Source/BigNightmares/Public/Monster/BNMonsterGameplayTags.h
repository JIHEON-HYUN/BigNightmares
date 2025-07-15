// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "BNMonsterGameplayTags.generated.h" // .generated.h 파일은 항상 마지막에 위치해야 합니다.

USTRUCT(BlueprintType)
struct FBNMonsterGameplayTags
{
	GENERATED_BODY()

public:
	static const FBNMonsterGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// --- 몬스터 상태 태그들 ---
	UPROPERTY()
	FGameplayTag Character_Monster_Dormant;

	UPROPERTY()
	FGameplayTag Character_Monster_Active;

	UPROPERTY()
	FGameplayTag Character_Monster_Active_Idle;

	UPROPERTY()
	FGameplayTag Character_Monster_Active_Chasing;

	UPROPERTY()
	FGameplayTag Character_Monster_Active_Attacking;

private:
	static FBNMonsterGameplayTags GameplayTags;
};