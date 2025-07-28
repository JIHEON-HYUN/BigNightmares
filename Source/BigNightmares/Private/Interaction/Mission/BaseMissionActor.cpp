// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/BaseMissionActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NaniteSceneProxy.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseMissionActor::ABaseMissionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//서버에 복제 허용
	bReplicates = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	SetRootComponent(ActorMesh);

	OverlapBox = CreateDefaultSubobject<UBoxComponent>("OverlapBox");
	OverlapBox->SetupAttachment(GetRootComponent());

	OverlapCapsule = CreateDefaultSubobject<UCapsuleComponent>("OverlapCapsule");
	OverlapCapsule->SetupAttachment(GetRootComponent());

	// 해당 컴포넌트가 모든 충돌 채널에 대한 충돌 응답을 설정 **지금의 경우 무시**
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 해당 컴포넌트가 특정 충돌 채널에 대한 충돌 응답을 설정 **Pawn에 대한 충돌은 OverLap을 감지**
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
}

// Called when the game starts or when spawned
void ABaseMissionActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//OverlapBox Component에 BeginOverlap이 발생하면 ABaseMissionActor에 정의된 OnBoxBeginOverlap 함수를 발동.
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseMissionActor::OnBeginOverlap);
	}
	
}

void ABaseMissionActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABaseMissionActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

UBoxComponent* ABaseMissionActor::GetBoxOverlapComponent() const
{
	return OverlapBox;
}

UCapsuleComponent* ABaseMissionActor::GetCapsuleOverlapComponent() const
{
	return OverlapCapsule;
}

UStaticMeshComponent* ABaseMissionActor::GetStaticMeshComponent() const
{
	return ActorMesh;
}

TSubclassOf<UGameplayEffect> ABaseMissionActor::GetOverlapEffect() const
{
	return OverlapEffect;
}
