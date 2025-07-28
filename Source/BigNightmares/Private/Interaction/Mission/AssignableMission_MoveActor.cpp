// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/AssignableMission_MoveActor.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "Player/BNMonoCharacter.h"

// Sets default values
AAssignableMission_MoveActor::AAssignableMission_MoveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	SetRootComponent(ActorMesh);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());

	SphereComponent->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));

	MoveNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MoveNiagara"));
	MoveNiagara->SetupAttachment(GetRootComponent());
	MoveNiagara->bAutoActivate = true;

	FinishNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StopNiagara"));
	FinishNiagara->SetupAttachment(GetRootComponent());
	FinishNiagara->bAutoActivate = false;
	
}

void AAssignableMission_MoveActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAssignableMission_MoveActor,bIsFinishNiagaraActive);
}

// Called when the game starts or when spawned
void AAssignableMission_MoveActor::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetVisibility(true);
	
	if (HasAuthority())
    {
    	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAssignableMission_MoveActor::OnBeginOverlap);
    }

	//Niagara Setting
	if (IsValid(MoveNiagara) && MoveNiagara->GetAsset()) // 에셋이 할당되었는지 확인
	{
		// Color User Parameter 설정
		MoveNiagara->SetColorParameter("Color", FLinearColor::Red);
	}

	if (IsValid(FinishNiagara) && FinishNiagara->GetAsset())
	{
		FinishNiagara->SetColorParameter("Color", FLinearColor::Red);
		FinishNiagara->SetFloatParameter("Intensity", 5.0f);
	}
}

void AAssignableMission_MoveActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ABNMonoCharacter* ActorType = Cast<ABNMonoCharacter>(OtherActor);
		if (IsValid(ActorType))
		{
			OnMissionActorDestroyed.Broadcast(this);
			//이곳에 level의 문이 열리는 로직만 있으면 끝!
			Destroy();	
		}		
	}
}

void AAssignableMission_MoveActor::SettingCollision()
{
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SphereComponent->SetVisibility(true);
}

void AAssignableMission_MoveActor::ChangeNiagara()
{
	if (HasAuthority()) // 서버인 경우 바로 실행
	{
		bIsFinishNiagaraActive = true; // FinishNiagara 켜기
		OnRep_IsFinishNiagaraActive();
	}
	else // 클라이언트인 경우 서버 RPC 호출
	{
		Server_SetFinishNiagaraState(true); // FinishNiagara 켜도록 서버에 요청
	}
}

void AAssignableMission_MoveActor::OnRep_IsFinishNiagaraActive()
{
	if (IsValid(FinishNiagara))
	{
		if (bIsFinishNiagaraActive)
		{
			FinishNiagara->Activate(true);
		}
	}
}

void AAssignableMission_MoveActor::Server_SetFinishNiagaraState_Implementation(bool bNewFinishState)
{
	// 서버에서 상태 변수를 변경 (클라이언트에 복제될 것임)
	bIsFinishNiagaraActive = bNewFinishState;

	// 서버에서도 OnRep 함수를 호출하여 로컬 상태 업데이트
	OnRep_IsFinishNiagaraActive();
}

bool AAssignableMission_MoveActor::Server_SetFinishNiagaraState_Validate(bool bNewFinishState)
{
	return true;
}

