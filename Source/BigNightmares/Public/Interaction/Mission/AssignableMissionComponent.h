// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AssignableMissionComponent.generated.h"

class AAssignableMission_MoveActor;
enum class EAssignableMissionMovementDirection : uint8;
class USplineComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIGNIGHTMARES_API UAssignableMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAssignableMissionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// 매 프레임 호출되는 Tick 함수 (움직임 로직 구현)
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Mission State Variables
public:
	//해당 미션의 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated, Category="Mission | ID")
	FGuid MissionID;

	//오버랩 되어 미션중인 플레이어 수
	UPROPERTY(ReplicatedUsing=OnRep_CurrentActivePlayerCount)
	int32 CurrentActivePlayerCount;

	UFUNCTION()
	void OnRep_CurrentActivePlayerCount();
	
#pragma endregion

#pragma region Movement Variables & Functions
public:
	// 이 컴포넌트가 참조할 스플라인 컴포넌트 (액터에서 할당)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Replicated, Category = "Spline Movement")
	TObjectPtr<USplineComponent> MovementSpline;

	// 이 컴포넌트가 움직일 액터 (스폰된 구체)
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Replicated, Category = "Spline Movement")
	TObjectPtr<AAssignableMission_MoveActor> ActorToMove;

	// 0~1 구간 플레이어 당 기본 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Speed")
	float PlayerBaseSpeed;

	// 0~1 구간에서 플레이어가 없을 때 되돌아가는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Speed")
	float ReturnSpeed;

	// 1~2 구간 고정 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Speed")
	float FixedSpeed;

protected:
	UPROPERTY(Replicated)
	float CurrentSplineDistance;

	// 현재 이동 방향 (복제)
	UPROPERTY(ReplicatedUsing=OnRep_MovementDirection)
	EAssignableMissionMovementDirection CurrentMovementDirection;

	// CurrentMovementDirection이 복제될 때 호출될 함수
	UFUNCTION()
	void OnRep_MovementDirection();

public:
	// 스플라인 이동을 업데이트하는 함수 (Tick에서 호출)
	void UpdateMovement(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddPlayerOverlap(APlayerState* PlayerState);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemovePlayerOverlap(APlayerState* PlayerState);

	void SetMovementDirection(EAssignableMissionMovementDirection NewDirection);
#pragma endregion

	UFUNCTION()
	void OnMissionMoveActorDestroyed(AActor* DestroyedActor);
};

