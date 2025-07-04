// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BNBaseGameplayAbility.h"
#include "BNTarotCardGameplayAbility.generated.h"

class ABNMonoCharacter;

//TODO: 아이템 효과를 만약 몬스터에게 적용시키고 싶으면 몬스터의 정보를 가져와야함.

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNTarotCardGameplayAbility : public UBNBaseGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Ability")
	ABNMonoCharacter* GetTargetCharacter() const;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
