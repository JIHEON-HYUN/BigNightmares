// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Monster/BNHunterAIController.h"


ABNHunterCharacter::ABNHunterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ==========================
	// 콜리전 설정
	// ==========================
	GetCapsuleComponent()->InitCapsuleSize(34.f, 95.0f);


	// ==========================
	// 메쉬 위치 설정
	// ==========================
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	
	// ===============================
	// AI 컨트롤러 설정
	// ===============================
	AIControllerClass = ABNHunterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 로그는 추후에 삭제 예정
	UE_LOG(LogTemp, Warning, TEXT("[AI] %s 생성 완료"), *GetName());
	
	if (AController* MyController = GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AI] %s -> 컨트롤러 %s 할당됨"), *GetName(), *MyController->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AI] %s -> 컨트롤러 없음"), *GetName());
	}
}

void ABNHunterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (GetMonsterState())
	{
	case EMonsterState::Idle:
		HandleIdle();
		break;
	case EMonsterState::Chase:
		HandleChase();
		break;
	case EMonsterState::Attack:
		HandleAttack();
		break;
	default:
		break;
	}
}

void ABNHunterCharacter::HandleIdle()
{
	
}

void ABNHunterCharacter::HandleChase()
{
	
}

void ABNHunterCharacter::HandleAttack()
{
	
}