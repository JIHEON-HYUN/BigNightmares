// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "Interaction/Mission/BaseMissionActor.h"
#include "AssignableMissionActor.generated.h"

class AAssignableMission_EscapeGate;
class AAssignableMission_MoveActor;
class USplineComponent;
class UAssignableMissionComponent;
/**
 * 
 */

UCLASS()
class BIGNIGHTMARES_API AAssignableMissionActor : public ABaseMissionActor
{
	GENERATED_BODY()

public:
	AAssignableMissionActor();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAssignableMissionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAssignableMissionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region Value
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assignable | Value")
	TObjectPtr<UAssignableMissionComponent>	AssignableMissionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Path")
	TObjectPtr<USplineComponent> MovementSpline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Path")
	TSubclassOf<AAssignableMission_MoveActor> MovementActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Movement | Path")
	TObjectPtr<AAssignableMission_MoveActor> MovementActorInstance;
#pragma endregion

#pragma region Escape Gate Spwan
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Gate")
	TSubclassOf<AAssignableMission_EscapeGate> EscapeGateActorClass;
	
	// UPROPERTY()
	// TArray<TObjectPtr<ATargetPoint>> AvailableSpawnPoints;
	
	//미션 완료 시 탈출 게이트 스폰
	void SpawnEscapeGate();
#pragma endregion
};
