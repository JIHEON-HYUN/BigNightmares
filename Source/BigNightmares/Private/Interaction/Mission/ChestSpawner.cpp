// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestSpawner.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "Algo/RandomShuffle.h" // 이 헤더 파일을 추가해주세요.

AChestSpawner::AChestSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AChestSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 이 코드를 실행하는 액터가 서버 권한을 가졌을 때만 아래 로직을 실행합니다.
	if (HasAuthority())
	{
		// 1. 유효성 검사
		if (ChestClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ChestClass is not set in ChestSpawner."));
			return;
		}
		if (SpawnPoints.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnPoints array is empty in ChestSpawner."));
			return;
		}

		// 2. 스폰 포인트 배열을 무작위로 섞기
		Algo::RandomShuffle(SpawnPoints);

		// 3. 상자 스폰하기
		int32 SpawnCount = FMath::Min(NumberOfChestsToSpawn, SpawnPoints.Num());
		for (int32 i = 0; i < SpawnCount; ++i)
		{
			ATargetPoint* CurrentPoint = SpawnPoints[i];
			if (CurrentPoint)
			{
				FTransform SpawnTransform = CurrentPoint->GetActorTransform();
				GetWorld()->SpawnActor<AActor>(ChestClass, SpawnTransform);
			}
		}
	}
}