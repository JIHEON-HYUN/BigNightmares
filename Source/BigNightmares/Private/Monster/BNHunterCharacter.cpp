// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

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

	// 캐릭터 무브먼트 컴포넌트의 기본 최대 속도를 WalkSpeed로 설정합니다.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	// --- 4. 시각 감지(Sight) 설정 ---
	// 부모의 PerceptionComponent를 가져와 헌터에 맞게 설정합니다.
	if (PerceptionComponent)
	{
		SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
		if (SightConfig)
		{
			SightConfig->SightRadius = 1500.f;
			SightConfig->LoseSightRadius = 2000.f;
			SightConfig->PeripheralVisionAngleDegrees = 90.0f;
			SightConfig->SetMaxAge(5.0f);

			SightConfig->DetectionByAffiliation.bDetectEnemies = true;
			SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
			SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

			PerceptionComponent->ConfigureSense(*SightConfig);
			PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}
}

void ABNHunterCharacter::ActivateMonster()
{
	UE_LOG(LogTemp, Warning, TEXT("BNHunterCharacter::ActivateMonster - Timer fired! Calling Super.")); // [로그 추가]
    Super::ActivateMonster();
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// --- 무기 생성 및 장착 로직 ---
	if (DefaultWeaponClass)
	{
		if (UWorld* World = GetWorld()) // 월드가 유효한지 확인하는 구문 추가
		{
			EquippedWeapon = World->SpawnActor<ABNMonsterWeapon>(DefaultWeaponClass);
			if (EquippedWeapon)
			{
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
		}
	}

	// --- 랜턴 생성 및 장착 로직 ---
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

	 // --- 테스트용 자동 활성화 로직 ---
    UE_LOG(LogTemp, Warning, TEXT("BNHunterCharacter::BeginPlay - Setting up activation timer.")); // [로그 추가]
    GetWorld()->GetTimerManager().SetTimer(
        ActivateTimerHandle,
        this,
        &ABNHunterCharacter::ActivateMonster,
        5.0f,
        false
    );
}

void ABNHunterCharacter::EnterIdleState()
{
	// 부모의 상태 전환 로직(태그 변경 등)을 먼저 실행합니다.
	Super::EnterIdleState();

	// 이동 속도를 일반 걷기 속도로 변경합니다.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNHunterCharacter::EnterChasingState()
{
	// 부모의 상태 전환 로직(태그 변경 등)을 먼저 실행합니다.
	Super::EnterChasingState();

	// 이동 속도를 추격 속도로 변경합니다.
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void ABNHunterCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();
}



