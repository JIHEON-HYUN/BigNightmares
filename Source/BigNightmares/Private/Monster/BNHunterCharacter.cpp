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

	// 캐릭터 무브먼트 컴포넌트의 기본 최대 속도를 WalkSpeed로 설정합니다.
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNHunterCharacter::ActivateMonster()
{
	// --- [추가된 디버그 코드 시작] ---
	AController* MyController = GetController();
	if (MyController)
	{
		// 현재 캐릭터의 이름과, 이 캐릭터에 빙의된 컨트롤러의 이름을 함께 출력합니다.
		UE_LOG(LogTemp, Log, TEXT("Log: Character '%s' is possessed by Controller '%s'"), *GetName(), *MyController->GetName());

		// 만약 컨트롤러가 AI 컨트롤러가 맞는지 더 확실하게 확인하고 싶다면, 캐스팅을 시도해볼 수 있습니다.
		if (ABNBaseAIController* MyAIController = Cast<ABNBaseAIController>(MyController))
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
		// 이 로그가 출력된다면, ActivateMonster가 호출되는 시점에 아직 컨트롤러가 빙의되지 않았다는 의미입니다.
		UE_LOG(LogTemp, Error, TEXT("Error: Character '%s' has NO controller at the moment of activation!"), *GetName());
	}

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



