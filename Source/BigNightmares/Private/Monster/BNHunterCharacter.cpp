// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Monster/BNBaseAIController.h"
#include "Animation/AnimInstance.h"

ABNHunterCharacter::ABNHunterCharacter()
{
	// --- 1. 캐릭터 기본 속성 설정 ---
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// --- 2. 캡슐 컴포넌트(물리적 형태) 설정 ---
	GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f);
	GetCapsuleComponent()->SetCapsuleRadius(34.0f);

	// --- 3. 메시 컴포넌트(외형) 설정 ---
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void ABNHunterCharacter::ActivateMonster()
{
	AController* MyController = GetController();
	if (MyController)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Character '%s' is possessed by Controller '%s'"), *GetName(), *MyController->GetName());
		if (Cast<ABNBaseAIController>(MyController))
		{
			UE_LOG(LogTemp, Log, TEXT("Log: Controller is confirmed to be an ABNBaseAIController."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error: Controller is NOT an ABNBaseAIController!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Character '%s' has NO controller at the moment of activation!"), *GetName());
	}
	Super::ActivateMonster();
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	if (DefaultWeaponClass)
	{
		if (UWorld* World = GetWorld())
		{
			EquippedWeapon = World->SpawnActor<ABNMonsterWeapon>(DefaultWeaponClass);
			if (EquippedWeapon)
			{
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
		}
	}

	if (DefaultLanternClass)
	{
		if (UWorld* World = GetWorld())
		{
			EquippedLantern = World->SpawnActor<AActor>(DefaultLanternClass);
			if (EquippedLantern)
			{
				EquippedLantern->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LanternAttachSocketName);
			}
		}
	}
}

void ABNHunterCharacter::EnterIdleState()
{
	Super::EnterIdleState();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNHunterCharacter::EnterChasingState()
{
	Super::EnterChasingState();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void ABNHunterCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();

	// 이 함수의 역할은 이제 순수하게 몽타주를 재생하는 것 뿐입니다.
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}
}