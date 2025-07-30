// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNMonsterSpawner.h"
#include "Character/BNBaseMonster.h"
#include "AI/Triggers/BNMonsterActivationTrigger.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "Algo/RandomShuffle.h"

ABNMonsterSpawner::ABNMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABNMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		return;

	if (!MonsterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MonsterClass is not set in BNMonsterSpawner."));
		return;
	}
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnPoints array is empty in BNMonsterSpawner."));
		return;
	}
	if (!ActivationTrigger)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivationTrigger is null in BNMonsterSpawner."));
		return;
	}

	Algo::RandomShuffle(SpawnPoints);
	int32 SpawnCount = FMath::Min(NumberOfMonstersToSpawn, SpawnPoints.Num());
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < SpawnCount; ++i)
	{
		ATargetPoint* CurrentPoint = SpawnPoints[i];
		if (!CurrentPoint)
			continue;

		FTransform SpawnTransform = CurrentPoint->GetActorTransform();
		ABNBaseMonster* SpawnedMonster = GetWorld()->SpawnActor<ABNBaseMonster>(MonsterClass, SpawnTransform, SpawnParams);
		
		if (SpawnedMonster)
		{
			// 트리거에 등록
			ActivationTrigger->AddMonsterToActivate(SpawnedMonster);
			UE_LOG(LogTemp, Log, TEXT("Spawned Monster: %s"), *SpawnedMonster->GetName());
		}
	}
}
