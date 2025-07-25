// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/AssignableMission_MoveActor.h"

#include "Components/SphereComponent.h"
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
}

void AAssignableMission_MoveActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ABNMonoCharacter* ActorType = Cast<ABNMonoCharacter>(OtherActor);
		if (IsValid(ActorType))
		{
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

