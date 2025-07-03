// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "Abilities/BNTarotCardAttributeSet.h"

ABNPlayerState::ABNPlayerState()
{
	UE_LOG(LogTemp, Warning, TEXT("ABNPlayerState Constructor Called"));

	BNBaseAbilitySystemComponent = Cast<UBNBaseAbilitySystemComponent>(
		CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("BNBaseAbilitySystemComponent"))
	);

	BNBaseAttributeSet = NewObject<UBNTarotCardAttributeSet>(BNBaseAbilitySystemComponent, UBNTarotCardAttributeSet::StaticClass());
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay PlayerState: %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("BNBaseAttributeSet: %p"), BNBaseAttributeSet.Get());
}

UBNBaseAbilitySystemComponent* ABNPlayerState::GetAbilitySystemComponent() const
{
	return BNBaseAbilitySystemComponent;
}

void ABNPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("PlayerState BeginPlay called"));
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet pointer: %p"), BNBaseAttributeSet.Get());
}

void ABNPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (!BNBaseAttributeSet)
	{
		BNBaseAttributeSet = NewObject<UBNTarotCardAttributeSet>(this, UBNTarotCardAttributeSet::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("BNBaseAttributeSet created in PostInitializeComponents"));
	}
}
