// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/AssignableMissionActor.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Interaction/Mission/AssignableMissionComponent.h"
#include "Interaction/Mission/AssignableMission_MoveActor.h"
#include "Net/UnrealNetwork.h"

AAssignableMissionActor::AAssignableMissionActor()
{
	GetBoxOverlapComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetBoxOverlapComponent()->SetVisibility(false);

	AssignableMissionComponent = CreateDefaultSubobject<UAssignableMissionComponent>("AssignableMissionComponent");
	AddOwnedComponent(AssignableMissionComponent);

	MovementSpline = CreateDefaultSubobject<USplineComponent>("MissionActorMovementSpline");
	MovementSpline->SetupAttachment(GetRootComponent());
}


void AAssignableMissionActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAssignableMissionActor, MovementActorInstance);
}

void AAssignableMissionActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetCapsuleOverlapComponent())
	{
		GetCapsuleOverlapComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAssignableMissionActor::OnAssignableMissionBeginOverlap);
		GetCapsuleOverlapComponent()->OnComponentEndOverlap.AddDynamic(this, &AAssignableMissionActor::OnAssignableMissionEndOverlap);
	}
	
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!MovementSpline)
		{
			UE_LOG(LogTemp, Error, TEXT("MovementSpline이 유효하지 않습니다. 액터를 스폰할 수 없습니다."));
			return;
		}

		if (!MovementActorClass)
		{
			UE_LOG(LogTemp, Error, TEXT("MovementActorClass가 할당되지 않았거나 유효하지 않습니다. 액터를 스폰할 수 없습니다."));
			return;
		}

		//TargetActorSpawn
		FVector InitialSpawnLocationWorld = MovementSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FRotator InitialSpawnRotationWorld = MovementSpline->GetRotationAtSplinePoint(0, ESplineCoordinateSpace::World);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		MovementActorInstance = GetWorld()->SpawnActor<AAssignableMission_MoveActor>(MovementActorClass, InitialSpawnLocationWorld, InitialSpawnRotationWorld, SpawnParameters);
		if (IsValid(MovementActorInstance))
		{
			if (IsValid(AssignableMissionComponent))
			{
				AssignableMissionComponent->MovementSpline = MovementSpline;
				AssignableMissionComponent->ActorToMove = MovementActorInstance;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Not IsValid AssignableMissionComponent"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Not IsValid MovementActorInstance"));
		}
	}
}

void AAssignableMissionActor::OnAssignableMissionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//부모 오버랩에 아무것도 정의 안되어있음 (캐릭터인지 검사하는거 넣을 만 할듯)
	//Super::OnBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (HasAuthority())
	{
		if (!IsValid(OtherActor))
		{
			UE_LOG(LogTemp, Error, TEXT("!IsValid(OtherActor)"));
			return;
		}
		// 오버랩한 액터가 플레이어 컨트롤러의 폰인지 확인
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);
		if (IsValid(OverlappingPawn))
		{
			APlayerState* OverlappingPlayerState = OverlappingPawn->GetPlayerState();
			if (IsValid(OverlappingPlayerState))
			{
				if (IsValid(AssignableMissionComponent))
				{
					AssignableMissionComponent->Server_AddPlayerOverlap(OverlappingPlayerState);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Not IsValid(OverlappingPawn)"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("!HasAuthority()"));
	}
}

void AAssignableMissionActor::OnAssignableMissionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() && IsValid(OtherActor))
	{
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);
		if (IsValid(OverlappingPawn))
		{
			APlayerState* OverlappingPlayerState = OverlappingPawn->GetPlayerState();
			if (IsValid(OverlappingPlayerState))
			{
				// AssignableMissionComponent를 통해 플레이어 수 감소 요청
				if (IsValid(AssignableMissionComponent))
				{
					AssignableMissionComponent->Server_RemovePlayerOverlap(OverlappingPlayerState);
				}
			}
		}
	}
}

