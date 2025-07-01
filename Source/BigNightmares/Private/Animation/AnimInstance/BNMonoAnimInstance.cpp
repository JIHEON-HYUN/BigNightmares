// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstance/BNMonoAnimInstance.h"

#include "Player/BNMonoCharacter.h"

void UBNMonoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningMonoCharacter = Cast<ABNMonoCharacter>(OwningCharacter);
	}
}

void UBNMonoAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (bHasAcceleration)
	{
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxState = false;		
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}
