// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "BNMonsterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMonsterAttributeSet : public UBNBaseAttributeSet
{
	GENERATED_BODY()

public:
	UBNMonsterAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UBNMonsterAttributeSet, AttackDamage)

protected:
	UFUNCTION()
	virtual void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
};
