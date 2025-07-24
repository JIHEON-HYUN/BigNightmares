// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Net/UnrealNetwork.h"

// 생성자: 컴포넌트를 생성하고 초기화합니다.
AChestActor::AChestActor()
{
    PrimaryActorTick.bCanEverTick = true;

    ChestBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBase"));
    RootComponent = ChestBase;

    ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLid"));
    ChestLid->SetupAttachment(ChestBase);

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);

    // [추가 시작] 프롬프트 위젯 컴포넌트 생성 및 설정
    PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
    PromptWidget->SetupAttachment(RootComponent);
    PromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // 상자 위쪽으로 위치 조정
    PromptWidget->SetWidgetSpace(EWidgetSpace::Screen); // 항상 화면을 바라보게 설정
    PromptWidget->SetDrawSize(FVector2D(100.f, 100.f)); // 위젯 크기 설정
    PromptWidget->SetVisibility(false); // 처음에는 숨겨둠
    // [추가 끝]
    
    bIsOpening = false;
    TargetLidPitch = -90.0f;
    bReplicates = true;
}

// [추가] 변수 복제 규칙을 정의합니다.
void AChestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsOpen과 bIsTheCorrectChest 변수가 네트워크를 통해 복제되도록 설정합니다.
    DOREPLIFETIME(AChestActor, bIsOpen);
    DOREPLIFETIME(AChestActor, bIsTheCorrectChest);
}

// 게임 시작 시 호출
void AChestActor::BeginPlay()
{
    Super::BeginPlay();
    InitialLidRotation = ChestLid->GetRelativeRotation();
}

void AChestActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(OtherActor);
    // [수정] 캐릭터가 유효하고, 그 캐릭터의 역할이 'KeyHolder'일 때만 아래 로직을 실행합니다.
    if (Character && Character->PlayerRole == EPlayerRole::KeyHolder)
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyHolder entered the interaction volume."));
        if (PromptWidget)
        {
            PromptWidget->SetVisibility(true);
        }
        Character->SetInteractableActor(this);
    }
}

void AChestActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(OtherActor);
    // [수정] 영역을 나간 캐릭터가 'KeyHolder'일 때만 UI를 끄고 상호작용 대상을 초기화합니다.
    if (Character && Character->PlayerRole == EPlayerRole::KeyHolder)
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyHolder exited the interaction volume."));
        if (PromptWidget)
        {
            PromptWidget->SetVisibility(false);
        }
        Character->SetInteractableActor(nullptr);
    }
}


// 매 프레임 호출
void AChestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsOpening)
    {
        FRotator CurrentRotation = ChestLid->GetRelativeRotation();
        FRotator TargetRotation = InitialLidRotation + FRotator(TargetLidPitch, 0.0f, 0.0f);
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
        ChestLid->SetRelativeRotation(NewRotation);

        if (FMath::IsNearlyEqual(CurrentRotation.Pitch, TargetLidPitch, 0.5f))
        {
            bIsOpening = false;
        }
    }
}

// [수정] 함수의 이름에 _Implementation을 추가합니다.
void AChestActor::Interact_Implementation(AActor* InteractingActor)
{
    if (!bIsOpen && InteractingActor && HasAuthority())
    {
        ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(InteractingActor);
        if (Character)
        {
            if (Character->PlayerRole == EPlayerRole::KeyHolder)
            {
                if (bIsTheCorrectChest)
                {
                    UE_LOG(LogTemp, Warning, TEXT("KeyHolder Interaction Success! Opening the correct chest."));
                    bIsOpen = true;
                    bIsOpening = true;
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("KeyHolder Interaction: This is not the correct chest."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Interaction Failed: Player role is not KeyHolder."));
            }
        }
    }
}