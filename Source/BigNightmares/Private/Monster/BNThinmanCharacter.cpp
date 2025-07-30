// BNThinmanCharacter.cpp

#include "Monster/BNThinmanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
// [수정] GameMode 대신 CoopMissionManager 헤더를 포함합니다.
#include "Interaction/Mission/BNCoopMissionManager.h"

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

void ABNThinmanCharacter::AnimNotify_KillAllPlayers()
{
	if (HasAuthority())
	{
		// [수정] GameMode 대신 월드의 CoopMissionManager를 찾아 함수를 호출합니다.
		ABNCoopMissionManager* MissionManager = Cast<ABNCoopMissionManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ABNCoopMissionManager::StaticClass())
		);

		if (MissionManager)
		{
			// 미션 실패 함수를 호출합니다.
			MissionManager->AllPlayersKilledByThinman(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Thinman: CoopMissionManager not found in the world!"));
		}
	}
}
