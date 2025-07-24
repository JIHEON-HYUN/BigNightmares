// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class BIGNIGHTMARES_API AChestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 상호작용을 처리할 함수
	void Interact();

protected:
	// UPROPERTY를 통해 에디터에서 보이고 수정할 수 있도록 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestLid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionVolume;

	// 상자 상태 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsOpen;

	// 뚜껑 열림 애니메이션 관련 변수
	bool bIsOpening;
	float TargetLidPitch;
	FRotator InitialLidRotation;
};
