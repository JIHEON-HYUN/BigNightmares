// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNThinmanAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABNThinmanAIController::ABNThinmanAIController()
{
	// 시각 감지(Sight) 설정을 생성하고 구성합니다.
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		// 씬맨은 시야가 매우 넓고 멀리 봅니다. (미니게임 밸런스에 맞게 조절 필요)
		SightConfig->SightRadius = 3000.f;
		SightConfig->LoseSightRadius = 3500.f;
		SightConfig->PeripheralVisionAngleDegrees = 120.0f;
		SightConfig->SetMaxAge(5.0f);

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
        
		// [핵심] 부모 클래스의 AIPerceptionComponent에 이 설정을 적용합니다.
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->ConfigureSense(*SightConfig);
			AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}
}
