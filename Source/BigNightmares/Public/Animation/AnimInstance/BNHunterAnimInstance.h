// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance/BNMonsterAnimInstance.h"
#include "BNHunterAnimInstance.generated.h"

class ABNHunterCharacter;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNHunterAnimInstance : public UBNMonsterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Reference")
	TObjectPtr<ABNHunterCharacter> OwningHunterCharacter;
};
