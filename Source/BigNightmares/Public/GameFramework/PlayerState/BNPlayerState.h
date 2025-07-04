// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "BNPlayerState.generated.h"

class UBNTarotCardAttributeSet;
class UBNBaseAttributeSet;
class UBNBaseAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABNPlayerState();
	virtual UBNBaseAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UBNBaseAbilitySystemComponent* GetBNBaseAbilitySystemComponent() const { return BNBaseAbilitySystemComponent.Get(); }
	FORCEINLINE UBNTarotCardAttributeSet* GetBNBaseAttributeSet() const {return BNBaseAttributeSet;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	TObjectPtr<UBNBaseAbilitySystemComponent> BNBaseAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem")
	TObjectPtr<UBNTarotCardAttributeSet> BNBaseAttributeSet;

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	
};
