// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestSpawner.generated.h"

class ATargetPoint;

UCLASS()
class BIGNIGHTMARES_API AChestSpawner : public AActor
{
	GENERATED_BODY()
public:
	AChestSpawner();

protected:
	// 게임이 시작될 때 호출되는 함수
	virtual void BeginPlay() override;

public:
	// 스폰할 상자 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> ChestClass;

	// 스폰 위치로 사용할 TargetPoint 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<ATargetPoint*> SpawnPoints;

	// 스폰할 상자의 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (ClampMin = "1"))
	int32 NumberOfChestsToSpawn = 3;
};
