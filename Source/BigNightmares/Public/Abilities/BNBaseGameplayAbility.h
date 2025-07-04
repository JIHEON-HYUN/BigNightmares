// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseType/BaseEnumType.h"
#include "BNBaseGameplayAbility.generated.h"

class UBNBaseAbilitySystemComponent;

UENUM(BlueprintType)
enum class EBaseAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Ability")
	UBNBaseAbilitySystemComponent* GetBNBaseAbilitySystemComponentFromActorInfo() const;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle);
	
	UFUNCTION(BlueprintPure, Category="Ability",  meta=(DisplayName = "Apply Gameplay Effect Spec Handle To TargetActor", ExpandEnumAsExecs="OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle, EBaseSuccessType& OutSuccessType);

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly, Category="BaseAbility")
	EBaseAbilityActivationPolicy AbilityActivationPolicy = EBaseAbilityActivationPolicy::OnTriggered;
};
