// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "Abilities/BNTarotCardAttributeSet.h"

ABNPlayerState::ABNPlayerState()
{
	BNBaseAbilitySystemComponent = CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("BNBaseAbilitySystemComponent"));
	BNBaseAttributeSet = CreateDefaultSubobject<UBNTarotCardAttributeSet>(TEXT("UBNTarotCardAttributeSet"));
}

UBNBaseAbilitySystemComponent* ABNPlayerState::GetAbilitySystemComponent() const
{
	return BNBaseAbilitySystemComponent;
}
