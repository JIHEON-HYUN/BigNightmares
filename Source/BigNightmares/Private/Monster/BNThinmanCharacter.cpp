// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNThinmanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h" // AI의 행동 로직(비헤이비어 트리)을 제어하기 위해 필요합니다.
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h" // [타이머 해결책] 타이머 사용을 위해 추가

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

// [수정] 불필요해진 BeginPlay 함수를 삭제했습니다.
// void ABNThinmanCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
// }

// [타이머 해결책] AI 컨트롤러가 빙의될 때 호출됩니다.
void ABNThinmanCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// BeginPlay()가 확실히 끝난 후에 상태를 변경하기 위해 짧은 지연 타이머를 설정합니다.
	// 이렇게 하면 BaseMonster가 상태를 Dormant로 덮어쓰는 것을 막을 수 있습니다.
	GetWorld()->GetTimerManager().SetTimer(
		InitialStateTimerHandle, 
		this, 
		&ABNThinmanCharacter::SetInitialStateToIdle, 
		0.1f, 
		false);
}

// [타이머 해결책] 타이머에 의해 호출되어 최종적으로 상태를 Idle로 설정합니다.
void ABNThinmanCharacter::SetInitialStateToIdle()
{
	// 이제 안전하게 Idle 상태로 진입할 수 있습니다.
	EnterIdleState();
}

// [핵심 기능] 빛에 맞았을 때 호출되는 함수의 실제 구현부입니다.
void ABNThinmanCharacter::OnHitByLight_Implementation(bool bIsLit)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController || !AIController->GetBrainComponent() || !AbilitySystemComponent)
	{
		return;
	}

	if (bIsLit)
	{
		// --- 빛에 맞았을 때 ---
		UE_LOG(LogTemp, Warning, TEXT("Thinman is hit by light! Stopping logic."));
		
		AbilitySystemComponent->AddReplicatedLooseGameplayTag(StunnedByLightTag);
		
		// [디버깅 로그 추가] Stun 태그가 추가되었는지 확인합니다.
		if (StunnedByLightTag.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Attempting to add Stun Tag: %s"), *StunnedByLightTag.ToString());
			if (AbilitySystemComponent->HasMatchingGameplayTag(StunnedByLightTag))
			{
				UE_LOG(LogTemp, Warning, TEXT("SUCCESS: Stun Tag is now active on Thinman."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("FAILURE: Stun Tag was NOT applied to Thinman."));
			}
		}
		
		AIController->GetBrainComponent()->StopLogic(TEXT("Stunned by light"));
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		// --- 빛에서 벗어났을 때 ---
		UE_LOG(LogTemp, Warning, TEXT("Thinman is no longer lit. Resuming logic."));

		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(StunnedByLightTag);

		// [디버깅 로그 추가] Stun 태그가 제거되었는지 확인합니다.
		if (StunnedByLightTag.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Attempting to remove Stun Tag: %s"), *StunnedByLightTag.ToString());
			if (!AbilitySystemComponent->HasMatchingGameplayTag(StunnedByLightTag))
			{
				UE_LOG(LogTemp, Warning, TEXT("SUCCESS: Stun Tag has been removed from Thinman."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("FAILURE: Stun Tag was NOT removed from Thinman."));
			}
		}
		
		AIController->GetBrainComponent()->RestartLogic();
	}
}

void ABNThinmanCharacter::EnterIdleState()
{
	Super::EnterIdleState();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("Thinman '%s' has successfully entered Idle State via Timer."), *GetName());
}

void ABNThinmanCharacter::EnterChasingState()
{
	Super::EnterChasingState();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}
