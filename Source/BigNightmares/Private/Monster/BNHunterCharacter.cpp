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
	// 최대 시야
	SightConfig->SightRadius = 800.f;
	// 타겟을 잃는 거리
	SightConfig->LoseSightRadius = 1000.f;
	// 시야각
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
	if (!Actor) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
		if (Distance <= SightConfig->SightRadius)
		{
			Target = Actor;
			bIsTracking = true;
			SetMonsterState(EMonsterState::Chase);
		}
	}
	else
	{
		Target = nullptr;
		bIsTracking = false;
		SetMonsterState(EMonsterState::Idle);
	}
}

void ABNHunterCharacter::HandleIdle()
{
	// BT 기반이면 아무 것도 하지 않거나 상태 유지를 위한 설정만
	UE_LOG(LogTemp, Log, TEXT("[%s] Idle 상태 유지"), *GetName());

	// 예: Blackboard에 Idle 플래그 설정 (추후 BT 서비스에서 참조)
	// if (BlackboardComponent)
	//     BlackboardComponent->SetValueAsBool("IsIdle", true);
}

void ABNHunterCharacter::HandleChase()
{
	// 추후 BT에서는 MoveTo를 Task로 처리할 것이므로, 여기선 Target 설정 등만 담당
	if (!Target) return;

	UE_LOG(LogTemp, Log, TEXT("[%s] 타겟 추적 중: %s"), *GetName(), *Target->GetName());

	// 추후 BT에서 이 타겟을 Blackboard에 Set 하게 되면 더 이상 이 함수는 필요 없어짐
}

void ABNHunterCharacter::HandleAttack()
{
	if (!Target) return;

	UE_LOG(LogTemp, Log, TEXT("[%s] 공격 중! 대상: %s"), *GetName(), *Target->GetName());

	// 추후에는 애니메이션 실행, BT Task_Attack으로 대체될 예정
}