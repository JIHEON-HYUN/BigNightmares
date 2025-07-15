// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities//BNMonsterAttributeSet.h"
#include "Net/UnrealNetwork.h"

UBNMonsterAttributeSet::UBNMonsterAttributeSet()
{
	// 기본 공격력 설정
	InitAttackDamage(1.0f);
} 

void UBNMonsterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 공격력만 리플리케이트합니다.
	DOREPLIFETIME_CONDITION_NOTIFY(UBNMonsterAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
}

void UBNMonsterAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBNMonsterAttributeSet, AttackDamage, OldAttackDamage);
}
