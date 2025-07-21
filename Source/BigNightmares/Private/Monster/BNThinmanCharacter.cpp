// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNThinmanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"

ABNThinmanCharacter::ABNThinmanCharacter()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(119.0f);
	GetCapsuleComponent()->SetCapsuleRadius(25.0f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -119.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);
}

// AI 컨트롤러가 빙의될 때 타이머를 설정하여 시작 상태 문제를 해결합니다.
void ABNThinmanCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 짧은 지연 후 SetInitialStateToIdle 함수를 호출하도록 타이머를 설정합니다.
	GetWorld()->GetTimerManager().SetTimer(
		InitialStateTimerHandle, 
		this, 
		&ABNThinmanCharacter::SetInitialStateToIdle, 
		0.1f, 
		false);
}

// 타이머에 의해 호출되어 최종적으로 상태를 Idle로 설정합니다.
void ABNThinmanCharacter::SetInitialStateToIdle()
{
	EnterIdleState();
}

void ABNThinmanCharacter::OnHitByLight_Implementation(bool bIsLit)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController || !AIController->GetBrainComponent() || !AbilitySystemComponent)
	{
		return;
	}

	if (bIsLit)
	{
		AbilitySystemComponent->AddReplicatedLooseGameplayTag(StunnedByLightTag);
		AIController->GetBrainComponent()->StopLogic(TEXT("Stunned by light"));
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(StunnedByLightTag);
		AIController->GetBrainComponent()->RestartLogic();
	}
}

void ABNThinmanCharacter::EnterIdleState()
{
	Super::EnterIdleState();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNThinmanCharacter::EnterChasingState()
{
	Super::EnterChasingState();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void ABNThinmanCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();

	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}
}
