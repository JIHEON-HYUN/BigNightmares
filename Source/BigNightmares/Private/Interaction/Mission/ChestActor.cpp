// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Player/BNMonoCharacter.h" 
#include "Player/BNPlayerRole.h"    
#include "Net/UnrealNetwork.h" // [추가] 변수 복제를 위해 필요합니다.

// 생성자: 컴포넌트를 생성하고 초기화합니다.
AChestActor::AChestActor()
{
    // Tick 함수가 매 프레임 호출되도록 설정합니다.
    PrimaryActorTick.bCanEverTick = true;

    // 컴포넌트 생성
    ChestBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBase"));
    RootComponent = ChestBase;

    ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLid"));
    ChestLid->SetupAttachment(ChestBase); 

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);

    InteractionVolume->SetRelativeLocation(FVector(0.0f, 20.0f, 40.0f));
    InteractionVolume->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    InteractionVolume->SetRelativeScale3D(FVector(2.0f, 2.0f, 1.5f));
    
    // 변수 초기화
    bIsOpen = false;
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