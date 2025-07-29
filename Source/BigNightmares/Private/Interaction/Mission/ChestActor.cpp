// ChestActor.cpp

#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "Kismet/GameplayStatics.h"

AChestActor::AChestActor()
{
	// Tick 함수를 사용하도록 true로 변경합니다.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// 1. 컴포넌트 생성 및 설정
	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	RootComponent = ChestMesh;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);

	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
	PromptWidget->SetupAttachment(RootComponent);
	PromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidget->SetVisibility(false);

	// 2. 콜리전 설정 (가장 안정적인 방식)
	InteractionVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	InteractionVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	InteractionVolume->SetGenerateOverlapEvents(true);

	// 흔들림 효과 변수 초기화
	bIsShaking = false;
	ShakeDuration = 0.3f;
	ShakeIntensity = 2.0f;
	ShakeTimer = 0.0f;
}

void AChestActor::BeginPlay()
{
	Super::BeginPlay();

	// 3. 이벤트 바인딩
	if (InteractionVolume)
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AChestActor::OnOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AChestActor::OnOverlapEnd);
	}
	
	// 흔들림 효과를 위해 메시의 원래 위치를 저장합니다.
	if(ChestMesh)
	{
		OriginalMeshLocation = ChestMesh->GetRelativeLocation();
	}
}

void AChestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 흔들리고 있는 상태라면 애니메이션을 처리합니다.
	if (bIsShaking)
	{
		ShakeTimer -= DeltaTime;
		if (ShakeTimer > 0.f && ChestMesh)
		{
			// 사인 함수를 이용해 좌우로 부드럽게 흔들리는 값을 계산합니다.
			float SineValue = FMath::Sin(ShakeTimer * 50.f); // 50.f는 흔들림 속도
			FVector ShakeOffset = FVector(0.f, SineValue * ShakeIntensity, 0.f);
			ChestMesh->SetRelativeLocation(OriginalMeshLocation + ShakeOffset);
		}
		else
		{
			// 시간이 다 되면 흔들림을 멈추고 원래 위치로 되돌립니다.
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

	UE_LOG(LogTemp, Warning, TEXT("[ChestActor] '%s'가 범위에 들어옴. 역할: %s"), *PlayerCharacter->GetName(), *UEnum::GetValueAsString(PlayerCharacter->PlayerRole));

	PlayerCharacter->SetInteractableActor(this);
	
	if (PlayerCharacter->IsLocallyControlled() && PlayerCharacter->PlayerRole == EPlayerRole::KeyHolder)
	{
		if(PromptWidget) PromptWidget->SetVisibility(true);
	}
}

void AChestActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (!PlayerCharacter) return;

	UE_LOG(LogTemp, Log, TEXT("[ChestActor] '%s'가 범위에서 나감."), *PlayerCharacter->GetName());
	
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
	if (PlayerCharacter && PlayerCharacter->PlayerRole == EPlayerRole::KeyHolder)
	{
		if (bIsTheCorrectChest)
		{
			UE_LOG(LogTemp, Error, TEXT("!!! [ChestActor] 진짜 상자 상호작용 성공 !!!"));
			
			if (ABNInGameGameMode* GameMode = GetWorld()->GetAuthGameMode<ABNInGameGameMode>())
			{
				GameMode->MissionComplete();
			}

			Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[ChestActor] 가짜 상자와 상호작용했습니다. 흔들림 효과를 재생합니다."));
			Multicast_PlayShakeEffect();
		}
	}
}

void AChestActor::Multicast_PlayShakeEffect_Implementation()
{
	if(bIsShaking) return; 
	
	bIsShaking = true;
	ShakeTimer = ShakeDuration;
}
