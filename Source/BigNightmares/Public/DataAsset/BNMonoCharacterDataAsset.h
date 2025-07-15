// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BNMonoCharacterDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT()
struct FMonoCharacterDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	//TODO(NOTE) : StartingAbility와 PassiveAbility는 사용 안할 수도 있음.
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartingAbility;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbility;
};
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMonoCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FMonoCharacterDefaultInfo> ClassDefaultInfoMap;
};
