// ChestActor.cpp

#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
// [수정] GameMode 대신 CoopMissionManager 헤더를 포함합니다.
#include "Interaction/Mission/BNCoopMissionManager.h"

AChestActor::AChestActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	RootComponent = ChestMesh;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);

	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
	PromptWidget->SetupAttachment(RootComponent);
	PromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidget->SetVisibility(false);

	InteractionVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InteractionVolume->SetGenerateOverlapEvents(true);

	bIsShaking = false;
	ShakeDuration = 0.3f;
	ShakeIntensity = 2.0f;
	ShakeTimer = 0.0f;
}

void AChestActor::BeginPlay()
{
	Super::BeginPlay();

	if (InteractionVolume)
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AChestActor::OnOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AChestActor::OnOverlapEnd);
	}
	
	if(ChestMesh)
	{
		OriginalMeshLocation = ChestMesh->GetRelativeLocation();
	}
}

void AChestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsShaking)
	{
		ShakeTimer -= DeltaTime;
		if (ShakeTimer > 0.f && ChestMesh)
		{
			float SineValue = FMath::Sin(ShakeTimer * 50.f);
			FVector ShakeOffset = FVector(0.f, SineValue * ShakeIntensity, 0.f);
			ChestMesh->SetRelativeLocation(OriginalMeshLocation + ShakeOffset);
		}
		else
		{
			bIsShaking = false;
			if(ChestMesh) ChestMesh->SetRelativeLocation(OriginalMeshLocation);
		}
	}
}

void AChestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChestActor, bIsTheCorrectChest);
}

void AChestActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (!PlayerCharacter) return;

	PlayerCharacter->SetOverlappedInteractable(this);
	
	if (PlayerCharacter->IsLocallyControlled() && PlayerCharacter->PlayerRole == EPlayerRole::KeyHolder)
	{
		if(PromptWidget) PromptWidget->SetVisibility(true);
	}
}

void AChestActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (!PlayerCharacter) return;
	
	PlayerCharacter->ClearOverlappedInteractable(this);

	if (PlayerCharacter->IsLocallyControlled())
	{
		if(PromptWidget) PromptWidget->SetVisibility(false);
	}
}

void AChestActor::Interact_Implementation(AActor* InteractingActor)
{
	if (!HasAuthority()) return;

	ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(InteractingActor);
	// [수정] 상호작용을 시도한 플레이어가 KeyHolder인지 여기서 직접 검사합니다.
	if (PlayerCharacter && PlayerCharacter->PlayerRole == EPlayerRole::KeyHolder)
	{
		if (bIsTheCorrectChest)
		{
			UE_LOG(LogTemp, Error, TEXT("!!! [ChestActor] 진짜 상자 상호작용 성공 !!!"));
			
			ABNCoopMissionManager* MissionManager = Cast<ABNCoopMissionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABNCoopMissionManager::StaticClass()));
			if (MissionManager)
			{
				MissionManager->MissionComplete();
			}

			Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ChestActor] 가짜 상자와 상호작용했습니다. 흔들림 효과를 재생합니다."));
			Multicast_PlayShakeEffect();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChestActor] Interaction failed. Interactor is not the KeyHolder."));
	}
}

void AChestActor::Multicast_PlayShakeEffect_Implementation()
{
	if(bIsShaking) return; 
	
	bIsShaking = true;
	ShakeTimer = ShakeDuration;
}
