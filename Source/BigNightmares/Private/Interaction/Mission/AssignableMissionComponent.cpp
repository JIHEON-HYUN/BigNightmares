// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/AssignableMissionComponent.h"

#include "BaseType/BaseEnumType.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAssignableMissionComponent::UAssignableMissionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//기본값 설정
	PlayerBaseSpeed = 50.f;
	ReturnSpeed = 20.f;
	FixedSpeed = 100.f;

	CurrentSplineDistance = 0.0f;
	CurrentActivePlayerCount = 0;
	CurrentMovementDirection = EAssignableMissionMovementDirection::Stop;
}

void UAssignableMissionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//미션관련 변수
	DOREPLIFETIME(UAssignableMissionComponent, MissionID);
	DOREPLIFETIME(UAssignableMissionComponent, CurrentActivePlayerCount);

	// 이동 관련 변수들을 복제하도록 등록
	DOREPLIFETIME(UAssignableMissionComponent, MovementSpline);
	DOREPLIFETIME(UAssignableMissionComponent, ActorToMove);
	DOREPLIFETIME(UAssignableMissionComponent, CurrentSplineDistance);
	DOREPLIFETIME(UAssignableMissionComponent, CurrentMovementDirection);
}


// Called when the game starts
void UAssignableMissionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority && IsValid(MovementSpline))
	{
		CurrentSplineDistance = MovementSpline->GetDistanceAlongSplineAtSplinePoint(0);
	}
	
}

void UAssignableMissionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_Authority) 
	{
		UpdateMovement(DeltaTime);
	}
}

void UAssignableMissionComponent::OnRep_CurrentActivePlayerCount()
{
	// 클라이언트에서 플레이어 수 변경에 따른 시각적/음향 효과 등을 처리
}

void UAssignableMissionComponent::OnRep_MovementDirection()
{
	//그냥 이동방향 변경되면 호출되는건데, 올라올떄는 줄이 끌어올려지는 사운드, 내려가면 내려가는 사운드 등등 재생가능
}

void UAssignableMissionComponent::UpdateMovement(float DeltaTime)
{
	if (!IsValid(MovementSpline) || !IsValid(ActorToMove))
	{
		return;
	}
	
	float Speed = 0.0f;
	float SplineLength = MovementSpline->GetSplineLength();
	float TargetSplineDistance = CurrentSplineDistance;

	// 스플라인 1번 지점의 거리
	float SplinePoint1Distance = MovementSpline->GetDistanceAlongSplineAtSplinePoint(1);

	switch (CurrentMovementDirection)
	{
		case EAssignableMissionMovementDirection::Forward:
			{
				if (CurrentSplineDistance < SplinePoint1Distance)
				{
					Speed = PlayerBaseSpeed * FMath::Max(1, CurrentActivePlayerCount);
					TargetSplineDistance += Speed * DeltaTime;
					TargetSplineDistance = FMath::Min(TargetSplineDistance, SplinePoint1Distance);
				}
				else
				{
					Speed = FixedSpeed;
					TargetSplineDistance += Speed * DeltaTime;
					TargetSplineDistance = FMath::Min(TargetSplineDistance, SplineLength);
				}
				break;
			}

		case EAssignableMissionMovementDirection::Backward:
			{
				if (CurrentSplineDistance > 0.0f) // 0번 지점까지 되돌아가기
				{
					Speed = ReturnSpeed;
					TargetSplineDistance -= Speed * DeltaTime;
					TargetSplineDistance = FMath::Max(TargetSplineDistance, 0.0f); // 0번 지점 아래로 내려가지 않도록
				}
				break;
			}
		case EAssignableMissionMovementDirection::Stop:
		default:
			// 멈춤 상태, 아무것도 하지 않음
			break;
	}

	//최종 스플라인 업데이트
	CurrentSplineDistance = TargetSplineDistance;

	// 스플라인 거리에 따른 월드 위치/회전 계산
	FVector NewLocation = MovementSpline->GetLocationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);
	FRotator NewRotation = MovementSpline->GetRotationAtDistanceAlongSpline(CurrentSplineDistance, ESplineCoordinateSpace::World);

	// 액터 위치/회전 설정 (서버에서만)
	ActorToMove->SetActorLocationAndRotation(NewLocation, NewRotation);

	if (CurrentMovementDirection == EAssignableMissionMovementDirection::Forward && CurrentSplineDistance >= SplineLength)
	{
		// 2번 지점 (스플라인 끝)에 도달하면 멈춤
		SetMovementDirection(EAssignableMissionMovementDirection::Stop);
	}
	else if (CurrentMovementDirection == EAssignableMissionMovementDirection::Backward && CurrentSplineDistance <= 0.0f)
	{
		// 0번 지점에 도달하면 멈춤
		SetMovementDirection(EAssignableMissionMovementDirection::Stop);
	}
    
	// 플레이어 수에 따른 방향 전환 로직 (서버에서만)
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentSplineDistance < SplinePoint1Distance) // 0~1 구간에 있을 때만 해당 로직 적용
		{
			if (CurrentActivePlayerCount == 0 && CurrentMovementDirection == EAssignableMissionMovementDirection::Forward)
			{
				// 플레이어가 없고 앞으로 가고 있었다면 되돌아가기
				SetMovementDirection(EAssignableMissionMovementDirection::Backward);
			}
			else if (CurrentActivePlayerCount > 0 && CurrentMovementDirection == EAssignableMissionMovementDirection::Backward)
			{
				SetMovementDirection(EAssignableMissionMovementDirection::Forward);
			}
			else if (CurrentActivePlayerCount > 0 && CurrentMovementDirection == EAssignableMissionMovementDirection::Stop && CurrentSplineDistance > 0.0f)
			{
				SetMovementDirection(EAssignableMissionMovementDirection::Forward);
			}
		}
	}

}

bool UAssignableMissionComponent::Server_AddPlayerOverlap_Validate(APlayerState* PlayerState)
{
	return IsValid(PlayerState); 
}

void UAssignableMissionComponent::Server_AddPlayerOverlap_Implementation(APlayerState* PlayerState)
{
	if (IsValid(PlayerState))
	{
		CurrentActivePlayerCount++;
        
		// 플레이어가 1명이라도 생기면 앞으로 이동 시작
		if (CurrentActivePlayerCount > 0 && CurrentMovementDirection != EAssignableMissionMovementDirection::Forward)
		{
			SetMovementDirection(EAssignableMissionMovementDirection::Forward);
		}
	}
}

bool UAssignableMissionComponent::Server_RemovePlayerOverlap_Validate(APlayerState* PlayerState)
{
	return IsValid(PlayerState); 
}

void UAssignableMissionComponent::Server_RemovePlayerOverlap_Implementation(APlayerState* PlayerState)
{
	if (IsValid(PlayerState) && CurrentActivePlayerCount > 0)
	{
		CurrentActivePlayerCount--;

		// 플레이어 수가 0이 되면 뒤로 이동 시작 (0~1 구간에 있을 경우)
		if (CurrentActivePlayerCount == 0 && CurrentSplineDistance < MovementSpline->GetDistanceAlongSplineAtSplinePoint(1))
		{
			SetMovementDirection(EAssignableMissionMovementDirection::Backward);
		}
	}
}


void UAssignableMissionComponent::SetMovementDirection(EAssignableMissionMovementDirection NewDirection)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentMovementDirection = NewDirection;
	}
}
