// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BNBaseAbilitySystemComponent.h"

void UBNBaseAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbility(AbilitySpec);
	}
}

void UBNBaseAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& PassivesToGrant)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToGrant)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UBNBaseAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	// [수정] AttributeEffect가 유효하지 않으면(nullptr이면) 크래시 대신 에러 로그를 남기고 함수를 안전하게 종료합니다.
	if (!AttributeEffect)
	{
		// 어떤 액터에서 문제가 발생했는지 알 수 있도록 로그에 액터 이름을 포함합니다.
		UE_LOG(LogTemp, Error, TEXT("AttributeEffect is not assigned for %s. Please set the default attribute GameplayEffect in the character's Blueprint."), *GetAvatarActor()->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Valid default Attribute for this character %s"), *GetAvatarActor()->GetName());
    
	const FGameplayEffectContextHandle ConTextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.f, ConTextHandle);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create a valid SpecHandle for %s!"), *GetAvatarActor()->GetName());
		return; // [추가] SpecHandle이 유효하지 않을 때도 안전하게 함수를 종료합니다.
	}

	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
