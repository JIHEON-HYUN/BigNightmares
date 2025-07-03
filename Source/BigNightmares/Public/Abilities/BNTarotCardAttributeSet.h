// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "BNTarotCardAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNTarotCardAttributeSet : public UBNBaseAttributeSet
{
	GENERATED_BODY()
	
public:
	UBNTarotCardAttributeSet();
	
	/**
	 * PreAttributeChange에서 태그를 기반으로 분기 하기 위해 플레이어의 ASC를 세팅할때 같은 값을 가져와서 사용
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AttributeAbilitySystemComponent = nullptr;

	void Init(UAbilitySystemComponent* InASC);

#pragma region MoveSpeedCard
	UPROPERTY(BlueprintReadOnly, Category="Status")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category="Status")
	FGameplayAttributeData MoveSpeedMultiplier;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, MoveSpeedMultiplier)

	UPROPERTY(BlueprintReadOnly, Category="Status")
	FGameplayAttributeData SpeedBuffDuration;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, SpeedBuffDuration)

	UPROPERTY()
	bool bShouldApplyMoveSpeed;
#pragma endregion

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void ChangeMoveSpeed(float NewSpeed);

};
