// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BNTarotCardGameplayAbility.h"

#include "Player/BNMonoCharacter.h"

ABNMonoCharacter* UBNTarotCardGameplayAbility::GetTargetCharacter() const
{
	if (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
	{
		return Cast<ABNMonoCharacter>(CurrentActorInfo->AvatarActor.Get());
	}
	return nullptr;
}

void UBNTarotCardGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), 1);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
	}
}
