// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "ChestActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UStaticMeshComponent; // [추가] UStaticMeshComponent의 존재를 알립니다.

UCLASS()
class BIGNIGHTMARES_API AChestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	AChestActor();

	// 인터페이스 함수
	virtual void Interact_Implementation(AActor* InteractingActor) override;

	// 블루프린트에서 호출 가능한 Overlap 이벤트 핸들러 함수
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 상태 변수
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool bIsTheCorrectChest = false;

protected:
	// Actor 생명주기 함수
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PromptWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestLid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionVolume;
	
	// 내부 상태 변수
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool bIsOpen;

	bool bIsOpening;
	float TargetLidPitch;
	FRotator InitialLidRotation;
};