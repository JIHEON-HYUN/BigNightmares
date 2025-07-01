// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance/BNCharacterAnimInstance.h"
#include "BNMonoAnimInstance.generated.h"

class ABNMonoCharacter;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMonoAnimInstance : public UBNCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;;

protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Reference")
	TObjectPtr<ABNMonoCharacter> OwningMonoCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Relax")
	bool bShouldEnterRelaxState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Relax")
	float EnterRelaxStateThreshold = 5.f;
	
	float IdleElapsedTime;
};
