// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNTarotCardAttributeSet.h"

ABNPlayerState::ABNPlayerState()
{
	NetUpdateFrequency = 100.f;     //초당 최대 네트워크 상태 업데이트 횟수
	MinNetUpdateFrequency = 60.f;	//상황에 따라 줄어들더라도 유지할 최소 업데이트 횟수 (최소 보장)
	
	BNBaseAbilitySystemComponent = Cast<UBNBaseAbilitySystemComponent>(
		CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("BNBaseAbilitySystemComponent"))
	);
	BNBaseAbilitySystemComponent->SetIsReplicated(true);	//네트워크에서 복제 되도록 설정
	BNBaseAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); //

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
