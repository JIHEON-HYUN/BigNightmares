// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/BNHunterAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

ABNHunterAIController::ABNHunterAIController()
{
	// ===============================
	// AI 감지 시스템 구성
	// ===============================

	// PerceptionComponent 생성 및 AIController에 연결
	UAIPerceptionComponent* Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*Perception);

	// 시야 감지 구성 생성 및 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;
	SightConfig->LoseSightRadius = 1500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);

	// 감지 대상 설정: 적만 감지
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	// 시야 감지를 PerceptionComponent에 적용
	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ABNHunterAIController::BeginPlay()
{
	Super::BeginPlay();

	// ===============================
	// BeginPlay 로직 (필요 시 추가)
	// ===============================
}
