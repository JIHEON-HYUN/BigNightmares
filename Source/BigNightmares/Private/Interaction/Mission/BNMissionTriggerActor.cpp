// BNMissionTriggerActor.cpp

#include "Interaction/Mission/BNMissionTriggerActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/Mission/BNCoopMissionManager.h"
#include "DrawDebugHelpers.h" // 디버그 드로잉을 위해 필요

ABNMissionTriggerActor::ABNMissionTriggerActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // [추가] 가장 안정적인 콜리전 설정
	InteractionVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

// [추가] BeginPlay에서 Overlap 이벤트를 바인딩합니다.
void ABNMissionTriggerActor::BeginPlay()
{
    Super::BeginPlay();
    if(HasAuthority())
    {
        InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ABNMissionTriggerActor::OnOverlapBegin);
        InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ABNMissionTriggerActor::OnOverlapEnd);
    }
}

void ABNMissionTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugBox(GetWorld(), InteractionVolume->GetComponentLocation(), InteractionVolume->GetScaledBoxExtent(), InteractionVolume->GetComponentRotation().Quaternion(), FColor::Red, false, 0, 1.0f);
}

// [추가] Overlap 시작 함수 구현
void ABNMissionTriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (PlayerCharacter)
	{
        // 캐릭터에게 상호작용 대상을 자기 자신으로 설정하도록 알립니다.
		PlayerCharacter->SetInteractableActor(this);
        UE_LOG(LogTemp, Warning, TEXT("[Trigger] %s entered interaction volume."), *PlayerCharacter->GetName());
	}
}

// [추가] Overlap 종료 함수 구현
void ABNMissionTriggerActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (PlayerCharacter)
	{
        // 캐릭터에게 상호작용 대상을 비우도록 알립니다.
		PlayerCharacter->ClearOverlappedInteractable(this);
        UE_LOG(LogTemp, Warning, TEXT("[Trigger] %s left interaction volume."), *PlayerCharacter->GetName());
	}
}

void ABNMissionTriggerActor::Interact_Implementation(AActor* InteractingActor)
{
	if (!HasAuthority()) return;

	ABNMonoCharacter* InteractingPlayer = Cast<ABNMonoCharacter>(InteractingActor);
	if (!InteractingPlayer) return;

	if (!FirstInteractor.IsValid())
	{
		FirstInteractor = InteractingPlayer;
		UE_LOG(LogTemp, Log, TEXT("Mission Trigger: %s activated. Waiting for another player."), *InteractingPlayer->GetName());
	}
	else if (InteractingPlayer != FirstInteractor.Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mission Trigger: Second player %s activated! Starting Co-op mission."), *InteractingPlayer->GetName());

		AActor* MissionManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABNCoopMissionManager::StaticClass());
		ABNCoopMissionManager* MissionManager = Cast<ABNCoopMissionManager>(MissionManagerActor);

		if (MissionManager)
		{
			MissionManager->StartMission(FirstInteractor.Get(), InteractingPlayer);
			Destroy(); 
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: CoopMissionManager NOT FOUND in the world!"));
		}
	}
}

