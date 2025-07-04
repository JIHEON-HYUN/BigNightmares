// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/BNHunterAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

ABNHunterAIController::ABNHunterAIController()
{
	// Perception Component 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
}

void ABNHunterAIController::SetupPerception()
{
	Super::SetupPerception();

	if (!SightConfig || !AIPerceptionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[HunterAI] SetupPerception 실패 - 컴포넌트 누락"));
		return;
	}
	else
	{
	    UE_LOG(LogTemp, Error, TEXT("[HunterAI] SetupPerception 성공"));
	}

	// 시야 설정
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1800.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	// Perception 구성
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// 감지 이벤트 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNHunterAIController::OnTargetPerceptionUpdated);

	UE_LOG(LogTemp, Log, TEXT("[HunterAI] SetupPerception 완료"));
}

void ABNHunterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Log, TEXT("[HunterAI] Actor %s 감지됨!"), *Actor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[HunterAI] Actor %s 시야에서 사라짐"), *Actor->GetName());
	}
}
