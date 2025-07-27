// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNThinmanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"

// 게임 모드와 플레이어 캐릭터에 접근하기 위해 헤더를 포함합니다.
#include "GameFramework/GameMode/BNCoopMissionGameMode.h"
#include "Kismet/GameplayStatics.h"

ABNThinmanCharacter::ABNThinmanCharacter()
{
	PrimaryActorTick.bCanEverTick = false; 

	GetCapsuleComponent()->SetCapsuleHalfHeight(119.0f);
	GetCapsuleComponent()->SetCapsuleRadius(25.0f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -119.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);
}

void ABNThinmanCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	// 이 함수는 이제 공격 몽타주를 재생하는 역할만 합니다.
	if (AttackMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (!AnimInstance->Montage_IsPlaying(AttackMontage))
			{
				AnimInstance->Montage_Play(AttackMontage);
			}
		}
	}
}

// [핵심] 애니메이션 노티파이가 이 함수를 호출하여 플레이어를 사망시킵니다.
void ABNThinmanCharacter::AnimNotify_KillAllPlayers()
{
	// 서버에서만 사망 로직을 실행하도록 보장합니다.
	if (HasAuthority())
	{
		// 현재 월드의 게임 모드를 가져옵니다.
		if (ABNCoopMissionGameMode* GameMode = Cast<ABNCoopMissionGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			// 게임 모드의 모든 플레이어 사망 함수를 호출합니다.
			GameMode->AllPlayersKilledByThinman(this);
		}
	}
}