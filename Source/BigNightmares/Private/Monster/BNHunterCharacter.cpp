// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNHunterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Perception/AISense_Sight.h"

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


	// ==========================
	// 감지 시스템 설정
	// ==========================	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 800.f;
	SightConfig->LoseSightRadius = 1000.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNHunterCharacter::OnTargetPerceived);
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

void ABNHunterCharacter::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
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
