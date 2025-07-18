// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Monster/BNBaseAIController.h"

ABNHunterCharacter::ABNHunterCharacter()
{
	// --- 1. 캐릭터 기본 속성 설정 ---
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// --- 2. 캡슐 컴포넌트(물리적 형태) 설정 ---
	GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f);
	GetCapsuleComponent()->SetCapsuleRadius(34.0f);

	// --- 3. 메시 컴포넌트(외형) 설정 ---
	// 메시의 위치와 회전만 C++에서 설정합니다.
	// 실제 스켈레탈 메시와 애니메이션은 블루프린트에서 할당합니다.
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void ABNHunterCharacter::ActivateMonster()
{
	// [추가된 부분] 몬스터 활성화 시점에 컨트롤러가 유효한지 확인하는 디버깅 로그입니다.
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

	// BeginPlay는 블루프린트 변수가 모두 로드된 후 호출됩니다.
	// 이 시점에서 캐릭터의 초기 이동 속도를 설정합니다.
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	// 무기 생성 및 장착 로직
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

	// 랜턴 생성 및 장착 로직
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

	// 이동 속도를 일반 걷기 속도로 변경합니다.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNHunterCharacter::EnterChasingState()
{
	Super::EnterChasingState();

	// 이동 속도를 추격 속도로 변경합니다.
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void ABNHunterCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();
}