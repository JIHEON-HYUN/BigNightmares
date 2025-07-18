// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/BNHunterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// 생성자
ABNHunterAIController::ABNHunterAIController()
{
	// 시각 감지 설정 컴포넌트 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		// 시야 반경 설정
		SightConfig->SightRadius = 1500.f;
		// 시야를 잃는 반경 설정
		SightConfig->LoseSightRadius = 2000.f;
		// 주변 시야각 설정
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		// 감지 정보 최대 유지 시간 설정
		SightConfig->SetMaxAge(5.0f);

		// 감지할 대상 설정 (적O, 중립O, 아군X)
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

		// 부모 클래스의 인지 컴포넌트 유효성 확인
		if (AIPerceptionComponent)
		{
			// 인지 컴포넌트에 시각 감지 설정 적용
			AIPerceptionComponent->ConfigureSense(*SightConfig);
			// 시각을 주 감각으로 설정
			AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}
}
