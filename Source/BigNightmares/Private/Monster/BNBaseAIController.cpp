// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "Perception/AIPerceptionComponent.h"


ABNBaseAIController::ABNBaseAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	// AI Controller의 주 지각 컴포넌트로 설정
	SetPerceptionComponent(*AIPerceptionComponent);

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseAIController::OnTargetPerceptionUpdated);
}

void ABNBaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABNBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// ===========================================
	// AI 지각 시스템 (AI Perception System)
	// ===========================================
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseAIController: Sensed actor: %s (Successfully: %d, Stimulus ID: %s)"), *Actor->GetName(), Stimulus.WasSuccessfullySensed(), *Stimulus.Tag.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseAIController: Lost sight of actor: %s (Successfully: %d, Stimulus ID: %s)"), *Actor->GetName(), Stimulus.WasSuccessfullySensed(), *Stimulus.Tag.ToString());
	}
}
