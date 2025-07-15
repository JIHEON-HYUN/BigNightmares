// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/BNHunterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

// 블랙보드 키 이름 정의
const FName ABNHunterAIController::TargetPlayerKey(TEXT("TargetPlayer"));

ABNHunterAIController::ABNHunterAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		SightConfig->SightRadius = 1000.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 200.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetClass());
	}
}

void ABNHunterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HunterAIController: BehaviorTreeAsset is not assigned!"));
	}
}

void ABNHunterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	Super::OnTargetPerceptionUpdated(Actor, Stimulus);

	// 감지된 액터가 플레이어인지 확인
	ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
	if (PlayerCharacter && PlayerCharacter != GetPawn()) 
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// [수정] 플레이어를 감지하면, 블랙보드의 'TargetPlayer' 키에 해당 액터를 저장합니다.
			UE_LOG(LogTemp, Warning, TEXT("HunterAIController: Player %s Sensed!"), *Actor->GetName());
			GetBlackboardComponent()->SetValueAsObject(TargetPlayerKey, Actor);
		}
		else // 플레이어를 놓쳤을 때
		{
			// [수정] 플레이어를 놓치면, 블랙보드의 'TargetPlayer' 키 값을 비웁니다.
			UE_LOG(LogTemp, Warning, TEXT("HunterAIController: Lost sight of Player %s!"), *Actor->GetName());
			GetBlackboardComponent()->ClearValue(TargetPlayerKey);
		}
	}
}
