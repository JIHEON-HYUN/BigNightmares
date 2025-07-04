// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/BNBaseAttributeSet.h"

UBNBaseAttributeSet::UBNBaseAttributeSet()
{
	
}

void UBNBaseAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
