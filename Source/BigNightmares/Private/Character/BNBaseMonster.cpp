// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Perception/AIPerceptionComponent.h"

ABNBaseMonster::ABNBaseMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	// ==========================
	// AI 감지 시스템 초기화
	// ==========================
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	// ==========================
	// 초기 상태 설정
	// ==========================
	CurrentState = EMonsterState::Inactive;
}

void ABNBaseMonster::BeginPlay()
{
	Super::BeginPlay();
}

// ==========================
// 외부 인터페이스
// ==========================

EMonsterState ABNBaseMonster::GetMonsterState() const
{
	UE_LOG(LogTemp, Log, TEXT("GetMonsterState 호출됨. 현재 상태: %s"), *UEnum::GetValueAsString(CurrentState));
	return CurrentState;
}

void ABNBaseMonster::ActivateMonster()
{
	if (bIsActive)
		return;

	bIsActive = true;
	SetMonsterState(EMonsterState::Idle);
}

// ==========================
// 상태 제어 내부 로직
// ==========================

void ABNBaseMonster::SetMonsterState(EMonsterState NewState)
{
	if (CurrentState == NewState)
		return;

	UE_LOG(LogTemp, Log, TEXT("%s 상태 변경: %s -> %s"),
		*GetName(),
		*UEnum::GetValueAsString(CurrentState),
		*UEnum::GetValueAsString(NewState));

	CurrentState = NewState;
}
