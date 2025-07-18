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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag DormantStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag IdleStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag ChaseStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag AttackStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State Tags")
	FGameplayTag AttackInProgressTag;
};
