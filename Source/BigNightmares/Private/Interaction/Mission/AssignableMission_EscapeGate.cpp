// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/AssignableMission_EscapeGate.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Player/BNMonoCharacter.h"

// Sets default values
AAssignableMission_EscapeGate::AAssignableMission_EscapeGate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	StaticMeshComponent->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
	
	EscapeTrigger = CreateDefaultSubobject<UBoxComponent>("EscapeTrigger");
	EscapeTrigger->SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->bAutoActivate = true;
}

// Called when the game starts or when spawned
void AAssignableMission_EscapeGate::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		EscapeTrigger->OnComponentBeginOverlap.AddDynamic(this, &AAssignableMission_EscapeGate::OnBeginOverlap);
	}

	if (IsValid(NiagaraComponent) && NiagaraComponent->GetAsset())
	{
		NiagaraComponent->SetVectorParameter("Position Offset", FVector(0, 0, 10));
	}
	
}

void AAssignableMission_EscapeGate::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
		if (IsValid(PlayerCharacter))
		{
			Server_GameCleared();

			Multicast_GameCleared();

			Destroy();
		}
	}
}

void AAssignableMission_EscapeGate::Multicast_GameCleared_Implementation()
{
	Server_GameCleared();
}

void AAssignableMission_EscapeGate::Server_GameCleared()
{
	// 이 함수에 게임 클리어 시 공통적으로 수행할 로직 넣기
	// 서버에서는 실제 게임 상태 변경 (예: 다음 레벨 로드, 플레이어 통계 업데이트)
	// 클라이언트에서는 UI 표시, 효과음 재생 등
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Game Cleared!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client: Game Cleared UI/FX!"));
	}
}
