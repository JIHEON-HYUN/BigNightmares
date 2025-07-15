// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "BNTarotCardAttributeSet.generated.h"

/**
 * 
 */

//TODO(NOTE) : UBNTarotCardAttributeSet이름 > UBNMonoCharacterAttributeSet 이었어야함.
UCLASS()
class BIGNIGHTMARES_API UBNTarotCardAttributeSet : public UBNBaseAttributeSet
{
	GENERATED_BODY()
	
public:
	UBNTarotCardAttributeSet();

	//서버에 내가 넘겨야할 값을 정의하는 곳 > ex) 계산이 완료된 MoveSpeed
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * PreAttributeChange에서 태그를 기반으로 분기 하기 위해 플레이어의 ASC를 세팅할때 같은 값을 가져와서 사용
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AttributeAbilitySystemComponent = nullptr;

	void Init(UBNBaseAbilitySystemComponent* InASC);
	
#pragma  region BaseMoveSpeed
	UPROPERTY(BlueprintReadOnly, Category="Status", ReplicatedUsing=OnRep_BaseMoveSpeed)
	FGameplayAttributeData BaseMoveSpeed;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, BaseMoveSpeed)

	//TODO(NOTE) : 속도의 지속시간을 감속과 가속시간을 다르게 하고 싶다면 해당 변수의 이름에 UP, DOWN을 추가하고 2개 만들어서 세팅. 지금은 동일한 시간
	UPROPERTY(BlueprintReadOnly, Category="Status", ReplicatedUsing=OnRep_SpeedBuffDuration)
	FGameplayAttributeData SpeedBuffDuration;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, SpeedBuffDuration)
	
#pragma region MoveSpeedUp
	UPROPERTY(BlueprintReadOnly, Category="Status", ReplicatedUsing=OnRep_MoveSpeedUpMultiplier)
	FGameplayAttributeData MoveSpeedUpMultiplier;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, MoveSpeedUpMultiplier)

#pragma endregion

#pragma region MoveSpeedDown
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Status")
	FGameplayAttributeData MoveSpeedDownMultiplier;
	ATTRIBUTE_ACCESSORS(UBNTarotCardAttributeSet, MoveSpeedDownMultiplier)
#pragma endregion

private:
	UPROPERTY()
	TMap<FGameplayAttribute, int32> AttributeChangeCount;

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void ChangeMoveSpeed(float NewSpeed,int32 Count = 0);

private:
	UFUNCTION()
	void OnRep_BaseMoveSpeed(const FGameplayAttributeData& OldSpeed);

	UFUNCTION()
	void OnRep_MoveSpeedUpMultiplier(const FGameplayAttributeData& UpMulti);

	UFUNCTION()
	void OnRep_SpeedBuffDuration(const FGameplayAttributeData& BuffDuration);


public:
	FORCEINLINE float GetMoveSpeed() const {return BaseMoveSpeed.GetCurrentValue();}
	
};
