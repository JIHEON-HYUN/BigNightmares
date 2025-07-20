// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNThinmanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"

ABNThinmanCharacter::ABNThinmanCharacter()
{
	// --- 1. 캡슐 컴포넌트(물리적 형태) 설정 ---
	GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f);
	GetCapsuleComponent()->SetCapsuleRadius(34.0f);

	// --- 2. 메시 컴포넌트(외형) 설정 ---
	// TODO(NOTE): Thinman의 실제 스켈레탈 메시 경로로 반드시 수정해야 합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Thinman/Meshes/SK_Thinman.SK_Thinman"));
	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// --- 3. 애니메이션 블루프린트 설정 ---
	// TODO(NOTE): Thinman의 실제 애니메이션 블루프린트 경로로 반드시 수정해야 합니다.
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClass(TEXT("/Game/Characters/Thinman/Animations/ABP_Thinman.ABP_Thinman_C"));
	if (AnimInstanceClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClass.Class);
	}
	
	// --- 4. 이동 속성 설정 ---
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 270.f, 0.f);
}

void ABNThinmanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 게임이 시작될 때의 초기 속도를 WalkSpeed로 설정합니다.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
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
		// --- 빛에 맞았을 때 ---
		AbilitySystemComponent->AddReplicatedLooseGameplayTag(StunnedByLightTag);
		AIController->GetBrainComponent()->StopLogic(TEXT("Stunned by light"));
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		// --- 빛에서 벗어났을 때 ---
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
