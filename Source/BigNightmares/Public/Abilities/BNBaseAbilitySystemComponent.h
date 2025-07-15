// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BNBaseAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNBaseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);
};
