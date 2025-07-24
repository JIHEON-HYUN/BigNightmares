// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "ChestActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class BIGNIGHTMARES_API AChestActor : public AActor, public IInteractionInterface 
{
	GENERATED_BODY()

public:
	// 생성자
	AChestActor();

	// 매 프레임 호출
	virtual void Tick(float DeltaTime) override;

	// [수정] 인터페이스 함수를 오버라이드합니다.
	virtual void Interact_Implementation(AActor* InteractingActor) override;

	// [추가] 이 상자가 열쇠로 열리는 진짜 상자인지 여부
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool bIsTheCorrectChest = false;

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

	// [추가] 변수 복제를 위해 추가
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestLid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	// 상자 상태 변수 (네트워크 복제를 위해 Replicated 추가)
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool bIsOpen;

	// 뚜껑 열림 애니메이션 관련 변수
	bool bIsOpening;
	float TargetLidPitch;
	FRotator InitialLidRotation;
};