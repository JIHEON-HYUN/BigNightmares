// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstance/BNMonsterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/BNBaseMonster.h"
#include "KismetAnimationLibrary.h"

void UBNMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningMonsterCharacter = Cast<ABNBaseMonster>(TryGetPawnOwner());
	if (OwningMonsterCharacter)
	{
		OwningMovementComponent = OwningMonsterCharacter->GetCharacterMovement();
	}
}

void UBNMonsterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!OwningMonsterCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningMonsterCharacter->GetVelocity().Size();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	if (GroundSpeed > 2.f)
	{
		FRotator ActorRot = OwningMonsterCharacter->GetActorRotation();
		FVector Velocity = OwningMonsterCharacter->GetVelocity();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRot);
	}
	else
	{
		Direction = 0.f;
	}
}

