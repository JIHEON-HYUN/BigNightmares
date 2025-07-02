// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"

ABNBaseMonster::ABNBaseMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	// 초기 상태는 Inactive
	SetMonsterState(EMonsterState::Inactive);
}

void ABNBaseMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ABNBaseMonster::SetMonsterState(EMonsterState NewState)
{
	if (CurrentState == NewState)
		return;

	CurrentState = NewState;
}

void ABNBaseMonster::ActivateMonster()
{
	if (bIsActive)
		return;

	bIsActive = true;
	SetMonsterState(EMonsterState::Idle);
}
