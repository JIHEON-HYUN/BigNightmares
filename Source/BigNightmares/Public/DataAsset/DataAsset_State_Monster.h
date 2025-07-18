// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DataAsset_State_Monster.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UDataAsset_State_Monster : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 휴면 상태 태그 (비활성화)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag DormantStateTag;

	// 대기 상태 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag IdleStateTag;

	// 추격 상태 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag ChaseStateTag;
	
	// 공격 상태 태그 (공격 시작)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag AttackStateTag;

	// 공격 진행 중 태그 (상태 변경 방지용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag AttackInProgressTag;
};
