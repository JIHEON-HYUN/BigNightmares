// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/ChestActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h" // PlaySoundAtLocation을 위해 추가

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
    
    // [수정] 애니메이션 변수 초기화
    bIsOpen = false;
    bIsOpening = false;
    TargetLidPitch = -90.0f; // 90도 열리도록 설정

    // 흔들림 변수 초기화
    bIsShaking = false;
    ShakeDuration = 0.3f; // 흔들리는 시간 (0.3초)
    ShakeIntensity = 3.0f;  // 흔들리는 강도

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
    // [추가] 게임이 시작되면, 뚜껑의 초기 회전 값을 저장합니다.
    InitialLidRotation = ChestLid->GetRelativeRotation();

    // 게임 시작 시, 상자 몸체의 원래 상대 위치를 저장합니다.
    if (ChestBase)
    {
        OriginalRelativeLocation = ChestBase->GetRelativeLocation();
    }
}

void AChestActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(OtherActor);
    // [수정] 캐릭터가 유효하고, 그 캐릭터의 역할이 'KeyHolder'일 때만 아래 로직을 실행합니다.
    if (Character && Character->PlayerRole == EPlayerRole::KeyHolder)
    {
        // [핵심 수정] UI 표시는 이 캐릭터를 "직접 조종하는 플레이어의 컴퓨터"에서만 실행합니다.
        if (Character->IsLocallyControlled())
        {
            if (PromptWidget)
            {
                PromptWidget->SetVisibility(true);
            }
        }
        // 상호작용 대상 설정은 모든 컴퓨터에서 해도 괜찮습니다.
        Character->SetInteractableActor(this);
    }
}

void AChestActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(OtherActor);
    // [수정] 영역을 나간 캐릭터가 'KeyHolder'일 때만 UI를 끄고 상호작용 대상을 초기화합니다.
    if (Character && Character->PlayerRole == EPlayerRole::KeyHolder)
    {
        // [핵심 수정] UI를 숨기는 것도 "직접 조종하는 플레이어의 컴퓨터"에서만 실행합니다.
        if (Character->IsLocallyControlled())
        {
            if (PromptWidget)
            {
                PromptWidget->SetVisibility(false);
            }
        }
        // 상호작용 대상을 비웁니다.
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
        FRotator TargetRotation = InitialLidRotation + FRotator(0.0f, 0.0f, TargetLidPitch); 
        // RInterpTo 함수가 매 프레임 뚜껑의 각도를 목표 각도까지 부드럽게 바꿔줍니다.
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
        // [핵심 수정] 회전 대상을 ChestBase가 아닌 ChestLid로 변경합니다.
        ChestLid->SetRelativeRotation(NewRotation);
        if (FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetLidPitch, 0.5f))
        {
            bIsOpening = false;
        }
    }

    // 흔들림 로직
    if (bIsShaking)
    {
        ShakeTimer -= DeltaTime;
        if (ShakeTimer > 0.f)
        {
            // 사인 함수를 이용해 좌우로 부드럽게 흔들리는 값을 계산합니다.
            float SineValue = FMath::Sin(ShakeTimer * 50.f); // 50.f는 흔들림 속도
            FVector ShakeOffset = FVector(0.f, SineValue * ShakeIntensity, 0.f);
            ChestBase->SetRelativeLocation(OriginalRelativeLocation + ShakeOffset);
        }
        else
        {
            // 시간이 다 되면 흔들림을 멈추고 원래 위치로 되돌립니다.
            bIsShaking = false;
            ChestBase->SetRelativeLocation(OriginalRelativeLocation);
        }
    }
}

// [추가] RepNotify 함수의 내용을 구현합니다.
void AChestActor::OnRep_IsOpen()
{
    // bIsOpen이 true로 변경되었다면, 애니메이션을 시작합니다.
    if (bIsOpen)
    {
        bIsOpening = true;

        // 상호작용 UI도 여기서 숨겨줍니다.
        if (PromptWidget)
        {
            PromptWidget->SetVisibility(false);
        }
    }
}

// 모든 클라이언트에게 흔들림 효과를 재생하라고 명령하는 멀티캐스트 함수의 실제 내용
void AChestActor::Multicast_PlayShakeEffect_Implementation()
{
    bIsShaking = true;
    ShakeTimer = ShakeDuration;
	
    // 여기에 "잠김" 또는 "덜컹" 소리를 추가하면 좋습니다.
    // 예: if (LockedSound) UGameplayStatics::PlaySoundAtLocation(this, LockedSound, GetActorLocation());
}

// [수정] 빠져있던 함수의 실제 내용을 추가합니다.
void AChestActor::Multicast_PlayOpenEffect_Implementation()
{
    bIsOpening = true;
    if (PromptWidget)
    {
        PromptWidget->SetVisibility(false);
    }
}

// [수정] 함수의 이름에 _Implementation을 추가합니다.
void AChestActor::Interact_Implementation(AActor* InteractingActor)
{
    // 서버에서만 실행되도록 HasAuthority() 체크를 유지합니다.
    if (!bIsOpen && InteractingActor && HasAuthority())
    {
        ABNMonoCharacter* Character = Cast<ABNMonoCharacter>(InteractingActor);
        if (Character && Character->PlayerRole == EPlayerRole::KeyHolder)
        {
            if (bIsTheCorrectChest) // 진짜 상자를 열었을 때
            {
                bIsOpen = true;
                OnRep_IsOpen(); // 서버에서도 즉시 실행
            }
            else // 가짜 상자를 열려고 했을 때
            {
                UE_LOG(LogTemp, Warning, TEXT("KeyHolder Interaction: This is the wrong chest. Playing shake effect."));
                Multicast_PlayShakeEffect(); // 모든 클라이언트에게 흔들림 효과 재생 명령
            }
        }
    }
}