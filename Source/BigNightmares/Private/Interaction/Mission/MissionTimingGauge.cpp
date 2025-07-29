// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/MissionTimingGauge.h"

#include "BaseType/BaseEnumType.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Interaction/Mission/VerticalTimingGaugeComponent.h"
#include "Player/BNMonoCharacter.h"
#include "UI/InGame/BNMission1Widget.h"

AMissionTimingGauge::AMissionTimingGauge()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCapsuleOverlapComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleOverlapComponent()->SetVisibility(false);
	
	TimingGaugeComponent = CreateDefaultSubobject<UVerticalTimingGaugeComponent>(TEXT("VerticalTimingGauge"));
	AddOwnedComponent(TimingGaugeComponent);

	MaxMissionLife = 3; // 기본 라이프 3
	RequiredSuccessCount = 4; // 기본 성공 횟수 4

	CurrentMissionLifeCount = MaxMissionLife;
	CurrentSuccessCount = 0;
}

void AMissionTimingGauge::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AMissionTimingGauge, CurrentMissionLifeCount, COND_OwnerOnly);
	DOREPLIFETIME(AMissionTimingGauge, MaxMissionLife);
	DOREPLIFETIME_CONDITION(AMissionTimingGauge, CurrentSuccessCount, COND_OwnerOnly);
	DOREPLIFETIME(AMissionTimingGauge, RequiredSuccessCount);
	
	DOREPLIFETIME(AMissionTimingGauge, CurrentChallengingPlayerState);
	DOREPLIFETIME(AMissionTimingGauge, CurrentPlayerController);
}

void AMissionTimingGauge::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && IsValid(GetBoxOverlapComponent()))
	{
		GetBoxOverlapComponent()->OnComponentEndOverlap.AddDynamic(this, &AMissionTimingGauge::OnOverlapEnd);
	}

	if (IsValid(TimingGaugeComponent))
	{
		if (!TimingGaugeComponent->GaugeID.IsValid())
		{
			TimingGaugeComponent->GaugeID = FGuid::NewGuid();
		}
	}

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(OverallMissionTimerHandle, this, &AMissionTimingGauge::HandleOverallMissionTimeOut, OverallMissionDuration, false);
	}
}

void AMissionTimingGauge::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor)
	{
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);
		if (!IsValid(OverlappingPawn))
		{
			return;
		}
		
		ABNPlayerController* BNPC = Cast<ABNPlayerController>(OverlappingPawn->GetController());
		if (!IsValid(BNPC))
		{
			UE_LOG(LogTemp, Error, TEXT("Server: Overlapping Pawn's Controller is Not a valid ABNPlayerController. OtherActor: %s"), *OtherActor->GetName());
			return;
		}
		
		ABNPlayerState* BNPS = Cast<ABNPlayerState>(OverlappingPawn->GetPlayerState());
		if (!IsValid(BNPS))
		{
			return;
		}
		
		ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
		if (!IsValid(BNGS) && !IsValid(TimingGaugeComponent))
		{
			return;
		}

		//GameState를 통해 미션 시작 시도
		if (BNGS->Server_TryStartSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, BNPC))
		{
			StartMission(BNPC);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MissionTimingGauge >> Server: Gauge ID %s is already active or cannot be started (by %s)."), *TimingGaugeComponent->GaugeID.ToString(), *BNPS->GetPlayerName());
		}
	}
}


void AMissionTimingGauge::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ABNMonoCharacter* OverlappingMonoCharacter = Cast<ABNMonoCharacter>(OtherActor);
		if (IsValid(OverlappingMonoCharacter))
		{
			if (IsValid(CurrentChallengingPlayerState)&& OverlappingMonoCharacter->GetPlayerState() == CurrentChallengingPlayerState)
			//미션 실패 or 성공 후 종료처리
			EndMission(EMissionResult::Failure);
		}
	}
}

/**
 *
 **/
#pragma region Server
void AMissionTimingGauge::HandleOverallMissionTimeOut()
{
	if (HasAuthority())
	{
		EndMission(EMissionResult::Failure);
	}
}

void AMissionTimingGauge::OnGaugeFinished(EVerticalGaugeResult Result)
{
	if (!HasAuthority()) return;

	if (Result == EVerticalGaugeResult::EVGR_Success)
	{
		Server_HandleMissionSuccess();
	}
	else if (Result == EVerticalGaugeResult::EVGR_Fail)
	{
		Server_HandleMissionFailure();
	}
	
	Server_CheckMissionCompletion();

	// 다음 게이지 주기를 시작할지 또는 미션이 완료되었는지 확인합니다.
	if (IsValid(CurrentPlayerController) && CurrentMissionLifeCount > 0 && CurrentSuccessCount < RequiredSuccessCount)
	{
		TimingGaugeComponent->RequestStartGauge(CurrentPlayerController);
	}
}

bool AMissionTimingGauge::Server_PerformGaugeCheck_Validate(FGuid InGaugeID, float ClientGaugeValue)
{
	if (!InGaugeID.IsValid())
	{
		return false;
	}

	//KINDA_SMALL_NUMBER : float의 값을 0과 비교할 때 미세한 오차 범위 내에 있는지 판단필요, 언리얼 엔진에 무시 가능한 오차를 측정할 때 사용하도록 만든 매크로
	if (ClientGaugeValue < -KINDA_SMALL_NUMBER || ClientGaugeValue > KINDA_SMALL_NUMBER + 1.0f)
	{
		return false;
	}
	
	return true;
}

void AMissionTimingGauge::Server_PerformGaugeCheck_Implementation(FGuid InGaugeID, float ClientGaugeValue)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (!IsValid(TimingGaugeComponent) || TimingGaugeComponent->GaugeID != InGaugeID)
	{
		return;
	}

	float ServerGreenZoneStartValue = TimingGaugeComponent->GreenZoneStart;
	float ServerGreenZoneEndValue = TimingGaugeComponent->GreenZoneStart + TimingGaugeComponent->GreenZoneLength;
	
	if (ClientGaugeValue >= ServerGreenZoneStartValue && ClientGaugeValue <= ServerGreenZoneEndValue)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SUCCESS!"));
		OnGaugeFinished(EVerticalGaugeResult::EVGR_Success);
		TimingGaugeComponent->UpdateDifficultySettings(CurrentSuccessCount);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed!"));
		OnGaugeFinished(EVerticalGaugeResult::EVGR_Fail);
	}
}

//서버 전용 로직들
void AMissionTimingGauge::Server_HandleMissionSuccess()
{
	if (!HasAuthority()) return;

	++CurrentSuccessCount;

	ABNPlayerController* LocalPC = Cast<ABNPlayerController>(GetOwner());
	if (IsValid(LocalPC) && LocalPC->IsLocalPlayerController())
	{
		// BNMission1Widget의 UI 업데이트 함수를 호출합니다.
		if (LocalPC->Mission1WidgetInstance) // 위젯 인스턴스가 있다면
		{
			LocalPC->Mission1WidgetInstance->UpdateSuccessUI(CurrentSuccessCount);
		}
	}
	
}

void AMissionTimingGauge::Server_HandleMissionFailure()
{
	if (!HasAuthority()) return;

	--CurrentMissionLifeCount;

	ABNPlayerController* LocalPC = Cast<ABNPlayerController>(GetOwner());
	if (IsValid(LocalPC) && LocalPC->IsLocalPlayerController())
	{
		if (LocalPC->Mission1WidgetInstance)
		{
			LocalPC->Mission1WidgetInstance->UpdateLifeUI(CurrentMissionLifeCount);
		}
	}
}

void AMissionTimingGauge::Server_CheckMissionCompletion()
{
	if (!HasAuthority()) return;

	if (CurrentMissionLifeCount <= 0)
	{
		EndMission(EMissionResult::Failure);
	}
	else if (CurrentSuccessCount >= RequiredSuccessCount)
	{
		EndMission(EMissionResult::Success);
	}
}

void AMissionTimingGauge::StartMission(ABNPlayerController* InPlayerController)
{
	if (!HasAuthority() || !IsValid(InPlayerController))
	{
		return;
	}

	//미션 변수설정
	CurrentPlayerController = InPlayerController;
	CurrentChallengingPlayerState = CurrentPlayerController->GetPlayerState<ABNPlayerState>();

	//이 액터의 Owner를 설정하여 COND_OwnerOnly 복제가 올바르게 작동
	SetOwner(CurrentPlayerController);

	//미션변수 초기화
	CurrentMissionLifeCount = MaxMissionLife;
	CurrentSuccessCount = 0;

	if (IsValid(CurrentPlayerController))
	{
		CurrentPlayerController->Client_ShowMission1GaugeUI( TimingGaugeComponent ,CurrentMissionLifeCount, CurrentSuccessCount);
		TimingGaugeComponent->RequestStartGauge(CurrentPlayerController);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(OverallMissionTimerHandle, this, &AMissionTimingGauge::HandleOverallMissionTimeOut, OverallMissionDuration, false);
	}
}

void AMissionTimingGauge::EndMission(EMissionResult Result)
{
	if (!HasAuthority())
	{
		return;
	}

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(OverallMissionTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(OverallMissionTimerHandle);
	}

	Client_EndMission(Result);

	ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
	if (IsValid(BNGS) && IsValid(TimingGaugeComponent))
	{
		// GameState에 이 게이지 챌린지가 종료되었음을 알림
		BNGS->Server_EndSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, CurrentPlayerController);
	}

	CurrentPlayerController = nullptr;
	CurrentChallengingPlayerState = nullptr;
	SetOwner(nullptr);

	if (Result == EMissionResult::Success)
	{
		//Destroy();
		const FString NewMeshPath = TEXT("/Game/MissionActor/Mission1/Box/OpenedBox/Mesh/Box_Open.Box_Open");
		UStaticMesh* OpenBoxMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *NewMeshPath));

		if (OpenBoxMesh)
		{
			GetStaticMeshComponent()->SetStaticMesh(OpenBoxMesh);
		}
		else
		{
			// 에러 로그: 메쉬 로드 실패
			UE_LOG(LogTemp, Warning, TEXT("Failed to load new mesh for TimingGaugeComponent."));
		}
		
		GetBoxOverlapComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (Result == EMissionResult::Failure)
	{
		//이펙트나 사운드등 여기에 작성하면 됨
	}
}
#pragma endregion

#pragma region Client

void AMissionTimingGauge::OnRep_CurrentMissionLifeCount()
{
	ABNPlayerController* LocalPC = Cast<ABNPlayerController>(GetOwner());
	if (IsValid(LocalPC) && LocalPC->IsLocalPlayerController())
	{
		if (LocalPC->Mission1WidgetInstance)
		{
			LocalPC->Mission1WidgetInstance->UpdateLifeUI(CurrentMissionLifeCount);
		}
	}
}

void AMissionTimingGauge::OnRep_CurrentSuccessCount()
{
	ABNPlayerController* LocalPC = Cast<ABNPlayerController>(GetOwner());
	if (IsValid(LocalPC) && LocalPC->IsLocalPlayerController())
	{
		// BNMission1Widget의 UI 업데이트 함수를 호출합니다.
		if (LocalPC->Mission1WidgetInstance) // 위젯 인스턴스가 있다면
		{
			LocalPC->Mission1WidgetInstance->UpdateSuccessUI(CurrentSuccessCount);
		}
	}
}

void AMissionTimingGauge::Client_EndMission_Implementation(EMissionResult Result)
{
	ABNPlayerController* LocalPC = Cast<ABNPlayerController>(GetOwner());
	if (IsValid(LocalPC) && LocalPC->IsLocalPlayerController())
	{
		if (IsValid(LocalPC->Mission1WidgetInstance))
		{
			LocalPC->Mission1WidgetInstance->RemoveFromParent();
		}

		FInputModeGameOnly InputMode;
		LocalPC->SetInputMode(InputMode);
		LocalPC->SetShowMouseCursor(false);
	}
}
#pragma endregion