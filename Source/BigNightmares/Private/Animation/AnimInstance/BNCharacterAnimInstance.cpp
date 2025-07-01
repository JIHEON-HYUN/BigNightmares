// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstance/BNCharacterAnimInstance.h"

#include "Character/BNBaseCharacter.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBNCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ABNBaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UBNCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	if (GroundSpeed > 2.f)
	{
		FRotator ActorRot = OwningCharacter->GetActorRotation();
		FVector Velocity = OwningCharacter->GetVelocity();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRot);
	}
	else
	{
		Direction=0;
	}
}
