// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Monster/BNBaseAIController.h"
#include "Animation/AnimInstance.h"

// 생성자
ABNHunterCharacter::ABNHunterCharacter()
{
	// --- 1. 캐릭터 기본 속성 설정 ---
	// 웅크린 상태에서 절벽 가장자리에서 떨어질 수 있도록 설정
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// --- 2. 캡슐 컴포넌트(물리적 형태) 설정 ---
	// 캡슐 컴포넌트 크기 설정
	GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f);
	GetCapsuleComponent()->SetCapsuleRadius(34.0f);

	// --- 3. 메시 컴포넌트(외형) 설정 ---
	// 메시 컴포넌트 위치, 회전, 콜리전 프로필 설정
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

// 몬스터 활성화 함수 (오버라이드)
void ABNHunterCharacter::ActivateMonster()
{
	// 현재 컨트롤러 가져오기
	AController* MyController = GetController();
	// 컨트롤러 유효성 확인 및 로그 출력
	if (MyController)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Character '%s' is possessed by Controller '%s'"), *GetName(), *MyController->GetName());
		// AI 컨트롤러 타입 확인 로그
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
	// 부모 클래스 함수 호출
	Super::ActivateMonster();
}

// 게임 시작 또는 스폰 시 호출
void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 이동 속도 초기화
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	// 기본 무기 클래스가 설정되었는지 확인
	if (DefaultWeaponClass)
	{
		if (UWorld* World = GetWorld())
		{
			// 월드에 무기 액터 스폰
			EquippedWeapon = World->SpawnActor<ABNMonsterWeapon>(DefaultWeaponClass);
			if (EquippedWeapon)
			{
				// 지정된 소켓에 무기 부착
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
		}
	}

	// 기본 랜턴 클래스가 설정되었는지 확인
	if (DefaultLanternClass)
	{
		if (UWorld* World = GetWorld())
		{
			// 월드에 랜턴 액터 스폰
			EquippedLantern = World->SpawnActor<AActor>(DefaultLanternClass);
			if (EquippedLantern)
			{
				// 지정된 소켓에 랜턴 부착
				EquippedLantern->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LanternAttachSocketName);
			}
		}
	}
}

// 대기 상태 진입 (오버라이드)
void ABNHunterCharacter::EnterIdleState()
{
	Super::EnterIdleState();
	// 상태에 맞는 이동 속도로 변경
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// 추격 상태 진입 (오버라이드)
void ABNHunterCharacter::EnterChasingState()
{
	Super::EnterChasingState();
	// 상태에 맞는 이동 속도로 변경
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

// 공격 상태 진입 (오버라이드)
void ABNHunterCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();

	// 공격 몽타주 유효성 확인
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		// 몽타주가 이미 재생 중인지 확인
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(AttackMontage);
		}
	}
}