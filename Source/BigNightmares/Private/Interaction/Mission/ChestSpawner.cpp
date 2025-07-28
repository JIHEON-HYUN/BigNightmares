// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestSpawner.h"
#include "Interaction/Mission/ChestActor.h" // [추가] AChestActor 클래스를 참조하기 위해 추가합니다.
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "Algo/RandomShuffle.h" 

AChestSpawner::AChestSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AChestSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 서버 권한을 가졌을 때만 로직을 실행합니다.
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

        // [수정] 스폰 로직을 수정하여 하나의 '진짜' 상자를 지정합니다.

		// 2. 스폰 포인트 배열을 무작위로 섞기
		Algo::RandomShuffle(SpawnPoints);

		// 3. 스폰할 상자 개수 결정
		int32 SpawnCount = FMath::Min(NumberOfChestsToSpawn, SpawnPoints.Num());
        
        // 4. '진짜' 상자가 될 인덱스를 무작위로 선택
        if (SpawnCount > 0)
        {
            int32 CorrectChestIndex = FMath::RandRange(0, SpawnCount - 1);
            UE_LOG(LogTemp, Warning, TEXT("Correct chest will be at spawn index: %d"), CorrectChestIndex);

            // 5. 상자 스폰하기
            for (int32 i = 0; i < SpawnCount; ++i)
            {
                ATargetPoint* CurrentPoint = SpawnPoints[i];
                if (CurrentPoint)
                {
                    FTransform SpawnTransform = CurrentPoint->GetActorTransform();
                    // SpawnActor 후 반환된 액터를 AChestActor로 형변환하여 저장합니다.
                    AChestActor* SpawnedChest = GetWorld()->SpawnActor<AChestActor>(ChestClass, SpawnTransform);

                    if (SpawnedChest)
                    {
                        // 6. 현재 스폰하는 상자가 '진짜' 상자인지 확인하고 플래그 설정
                        if (i == CorrectChestIndex)
                        {
                            SpawnedChest->bIsTheCorrectChest = true;
                            UE_LOG(LogTemp, Warning, TEXT("Spawned CORRECT chest: %s"), *SpawnedChest->GetName());
                        }
                        else
                        {
                            SpawnedChest->bIsTheCorrectChest = false;
                            UE_LOG(LogTemp, Log, TEXT("Spawned normal chest: %s"), *SpawnedChest->GetName());
                        }
                    }
                }
            }
        }
	}
}