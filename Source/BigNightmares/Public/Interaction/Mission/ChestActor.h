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

	// [수정] bIsOpen 변수가 복제될 때 OnRep_IsOpen 함수를 호출하도록 지정합니다.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_IsOpen, BlueprintReadOnly, Category = "State")
	bool bIsOpen;

	// [추가] bIsOpen의 RepNotify 함수를 선언합니다.
	UFUNCTION()
	void OnRep_IsOpen();

	// [추가] 모든 클라이언트에게 흔들림 효과를 재생하라고 명령하는 멀티캐스트 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayShakeEffect();

	// [추가] 모든 클라이언트에게 열림 효과를 재생하라고 명령하는 멀티캐스트 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayOpenEffect();

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PromptWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ChestLid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionVolume;

	// [추가] 뚜껑 열림 애니메이션 관련 변수들
	bool bIsOpening;
	float TargetLidPitch;
	FRotator InitialLidRotation;

	// [추가] 흔들림 애니메이션 관련 변수들
	bool bIsShaking;
	float ShakeDuration;
	float ShakeIntensity;
	float ShakeTimer;
	FVector OriginalRelativeLocation; // 원래 위치를 저장할 변수
};