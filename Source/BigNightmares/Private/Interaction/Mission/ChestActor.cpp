// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h" // Lerp 함수 사용을 위해 추가

// 생성자: 컴포넌트를 생성하고 초기화합니다.
AChestActor::AChestActor()
{
    // Tick 함수가 매 프레임 호출되도록 설정합니다.
    PrimaryActorTick.bCanEverTick = true;

    // 컴포넌트 생성
    ChestBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBase"));
    RootComponent = ChestBase; // 몸체를 루트 컴포넌트로 설정

    ChestLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLid"));
    ChestLid->SetupAttachment(ChestBase); // 뚜껑을 몸체에 부착

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);

    // 위치 설정 (X=0, Y=20, Z=40)
    InteractionVolume->SetRelativeLocation(FVector(0.0f, 20.0f, 40.0f));

    // 회전 설정 (Roll=0, Pitch=0, Yaw=0) - 기본값이지만 명시적으로 설정
    InteractionVolume->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

    // 스케일 설정 (X=2, Y=2, Z=1.5)
    InteractionVolume->SetRelativeScale3D(FVector(2.0f, 2.0f, 1.5f));
    
    // 변수 초기화
    bIsOpen = false;
    bIsOpening = false;
    TargetLidPitch = -90.0f; // 뚜껑을 -90도까지 열도록 설정
}

// 게임 시작 시 호출
void AChestActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 뚜껑의 초기 회전값 저장
    InitialLidRotation = ChestLid->GetRelativeRotation();
}

// 매 프레임 호출
void AChestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // bIsOpening 플래그가 true일 때만 뚜껑 회전 로직 실행
    if (bIsOpening)
    {
        // 현재 뚜껑의 회전값을 가져옴
        FRotator CurrentRotation = ChestLid->GetRelativeRotation();
        
        // 목표 회전값 설정 (초기 회전값에서 Pitch만 변경)
        FRotator TargetRotation = InitialLidRotation + FRotator(TargetLidPitch, 0.0f, 0.0f);
        
        // Lerp(보간)를 사용하여 부드럽게 회전
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
        
        ChestLid->SetRelativeRotation(NewRotation);

        // 목표 각도에 거의 도달하면 애니메이션 중지
        if (FMath::IsNearlyEqual(CurrentRotation.Pitch, TargetLidPitch, 0.5f))
        {
            bIsOpening = false;
        }
    }
}

// 상호작용 함수 구현
void AChestActor::Interact()
{
    // 상자가 닫혀 있을 때만 실행
    if (!bIsOpen)
    {
        UE_LOG(LogTemp, Warning, TEXT("Chest Interacted!"));
        bIsOpen = true;
        bIsOpening = true; // Tick에서 애니메이션을 시작하도록 플래그 설정

        // 여기에 사운드 재생이나 파티클 생성 코드를 추가할 수 있습니다.
        // UGameplayStatics::PlaySoundAtLocation(...);
    }
}