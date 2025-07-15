// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/BNHunterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABNHunterAIController::ABNHunterAIController()
{
	// 시각 감지(Sight) 설정을 생성하고 구성합니다.
	// 이 로직은 이전에 BNHunterCharacter에 있던 것입니다.
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

		// 부모 클래스(BNBaseAIController)의 AIPerceptionComponent에 이 설정을 추가합니다.
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->ConfigureSense(*SightConfig);
			AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}
}
