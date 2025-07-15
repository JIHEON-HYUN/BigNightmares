// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance/BNBaseAnimInstance.h"
#include "BNMonsterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ABNBaseMonster;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMonsterAnimInstance : public UBNBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<ABNBaseMonster> OwningMonsterCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category = "Locomotion")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category = "Locomotion")
	bool bHasAcceleration;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite, Category = "Locomotion")
	float Direction;
};
