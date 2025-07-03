// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

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
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();
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