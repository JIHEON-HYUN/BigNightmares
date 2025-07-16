// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNTarotCardAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Player/InventoryComponent.h"

ABNPlayerState::ABNPlayerState()
{
	NetUpdateFrequency = 100.f;     //초당 최대 네트워크 상태 업데이트 횟수
	MinNetUpdateFrequency = 66.f;	//상황에 따라 줄어들더라도 유지할 최소 업데이트 횟수 (최소 보장)
	
	BNBaseAbilitySystemComponent = CreateDefaultSubobject<UBNBaseAbilitySystemComponent>("AbilitySystemComponent");
	BNBaseAbilitySystemComponent->SetIsReplicated(true);	//네트워크에서 복제 되도록 설정
	BNBaseAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); //
	
	BNBaseAttributeSet = CreateAttributeSet<UBNTarotCardAttributeSet>("BNTarotCardAttributeSet");
	
	if (!IsValid(BNBaseAttributeSet))
	{
		UE_LOG(LogTemp, Error, TEXT("IsValid(BNBaseAttributeSet) : NOT!!!!"));			
	}
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay PlayerState: %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("BNBaseAttributeSet: %p"), BNBaseAttributeSet.Get());

	UE_LOG(LogTemp, Warning, TEXT("BNBaseAttributeSet 생성됨: %p"), BNBaseAttributeSet.Get());
	UE_LOG(LogTemp, Warning, TEXT("Class: %s"), *GetClass()->GetName());

	bReplicates = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
	
}

void ABNPlayerState::SetPlayerType(EPlayerType NewType)
{
	PlayerType = NewType;
}

UAbilitySystemComponent* ABNPlayerState::GetAbilitySystemComponent() const
{
	return BNBaseAbilitySystemComponent;
}

UBNBaseAbilitySystemComponent* ABNPlayerState::GetBNBaseAbilitySystemComponent() const
{
	return BNBaseAbilitySystemComponent;
}

UBNBaseAttributeSet* ABNPlayerState::GetBNBaseAttributeSet() const
{
	return BNBaseAttributeSet.Get();
}

void ABNPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("PlayerState BeginPlay called"));
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet pointer: %p"), GetBNBaseAttributeSet());

	UE_LOG(LogTemp, Warning, TEXT("IsServer: %s"), HasAuthority() ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet in BeginPlay: %p"), GetBNBaseAttributeSet());
}

void ABNPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABNPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABNPlayerState, InventoryComponent);
}

UInventoryComponent* ABNPlayerState::GetInventoryComponent_Implementation()
{
	return InventoryComponent;	
}
