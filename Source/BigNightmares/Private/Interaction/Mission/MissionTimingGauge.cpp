// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/MissionTimingGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
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

	if (HasAuthority() && IsValid(GetOverlapComponent()))
	{
		GetOverlapComponent()->OnComponentEndOverlap.AddDynamic(this, &AMissionTimingGauge::OnOverlapEnd);
	}

	if (IsValid(TimingGaugeComponent))
	{
		if (!TimingGaugeComponent->GaugeID.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("AMissionTimingGauge '%s': TimingGaugeComponent's GaugeID is not set! Please set a unique ID in the editor."), *GetName());
			TimingGaugeComponent->GaugeID = FGuid::NewGuid();
		}
	}

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(OverallMissionTimerHandle, this, &AMissionTimingGauge::HandleOverallMissionTimeOut, OverallMissionDuration, false);
	}
}

void AMissionTimingGauge::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
			UE_LOG(LogTemp, Warning, TEXT("Server: Overlapping Pawn's Controller is Not a valid ABNPlayerController. OtherActor: %s"), *OtherActor->GetName());
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
			UE_LOG(LogTemp, Warning, TEXT("시도함"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MissionTimingGauge >> Server: Gauge ID %s is already active or cannot be started (by %s)."), *TimingGaugeComponent->GaugeID.ToString(), *BNPS->GetPlayerName());
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
			//미션 실패 or 성공 후 종료처리
			EndMission(EMissionResult::Failure);
		}
	}
}

void AMissionTimingGauge::OnGaugeFinished(EVerticalGaugeResult Result)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("Server: Gauge ID '%s' finished with Result: %s"), 
			*TimingGaugeComponent->GaugeID.ToString(), 
			Result == EVerticalGaugeResult::EVGR_Success ? TEXT("Success") : TEXT("Fail")
		);

		if (Result == EVerticalGaugeResult::EVGR_Fail)
		{
			if (CurrentMissionLifeCount > 0 && CurrentSuccessCount < RequiredSuccessCount)
			{
				--CurrentMissionLifeCount; // 라이프 감소
			}			
		}

		CheckMissionCompletion();

		// 다음 게이지 주기를 시작할지 또는 미션이 완료되었는지 확인합니다.
		if (IsValid(CurrentPlayerController) && CurrentMissionLifeCount > 0 && CurrentSuccessCount < RequiredSuccessCount)
		{
			TimingGaugeComponent->RequestStartGauge(CurrentPlayerController);
			UE_LOG(LogTemp, Log, TEXT("Server: Starting next gauge cycle for %s."), *CurrentPlayerController->GetName());
		}
		
		// if (IsValid(CurrentChallengingPlayerState))
		// {
		// 	ABNPlayerController* BNPC = Cast<ABNPlayerController>(CurrentChallengingPlayerState->GetPlayerController());
		// 	if (BNPC)
		// 	{
		// 		ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
		// 		if (BNGS && IsValid(TimingGaugeComponent))
		// 		{
		// 			BNGS->Server_EndSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, BNPC);
		// 		}
		// 	}
		//
		// 	// if (Result == EVerticalGaugeResult::EVGR_Success)
		// 	// {
		// 	// 	if (GetOverlapEffect() && IsValid(CurrentChallengingPlayerState) &&  IsValid(CurrentChallengingPlayerState->GetPawn()))
		// 	// 	{
		// 	// 		if (UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CurrentChallengingPlayerState->GetPawn()))
		// 	// 		{
		// 	// 			const FGameplayEffectContextHandle ContextHandle = OtherASC->MakeEffectContext();
		// 	// 			const FGameplayEffectSpecHandle SpecHandle = OtherASC->MakeOutgoingSpec(GetOverlapEffect(), 1.f, ContextHandle);
		// 	// 			OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		// 	// 			UE_LOG(LogTemp, Log, TEXT("Server: Applied GameplayEffect for success to player %s."), *CurrentChallengingPlayerState->GetPlayerName());
		// 	// 		}
		// 	// 		else
		// 	// 		{
		// 	// 			UE_LOG(LogTemp, Warning, TEXT("Server: Failed to get AbilitySystemComponent for player %s."), *CurrentChallengingPlayerState->GetPlayerName());
		// 	// 		}
		// 	// 	}
		// 	// 	else
		// 	// 	{
		// 	// 		UE_LOG(LogTemp, Warning, TEXT("Server: OverlapEffect is not set or ChallengingPlayerState/Pawn is invalid for success."));
		// 	// 	}
		// 	// 	HandleMissionSuccess();
		// 	// 	
		// 	// 	Destroy()
		// }
	}
}


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

void AMissionTimingGauge::HandleOverallMissionTimeOut()
{
	if (HasAuthority())
	{
		EndMission(EMissionResult::Failure);
	}
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
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS!"));
		UE_LOG(LogTemp, Warning, TEXT("CurrentPointerYPosition : %f , %f ~ %f "), ClientGaugeValue, ServerGreenZoneStartValue, ServerGreenZoneEndValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed!"));
		UE_LOG(LogTemp, Warning, TEXT("CurrentPointerYPosition : %f , %f ~ %f "), ClientGaugeValue, ServerGreenZoneStartValue, ServerGreenZoneEndValue);
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

void AMissionTimingGauge::CheckMissionCompletion()
{
	if (HasAuthority())
	{
		//미션 성공
		if (CurrentSuccessCount >= RequiredSuccessCount)
		{
			EndMission(EMissionResult::Success);
		}
		else if (CurrentMissionLifeCount <= 0) //실패
		{
			EndMission(EMissionResult::Failure);
		}
	}
}

void AMissionTimingGauge::HandleMissionSuccess()
{
}

void AMissionTimingGauge::HandleMissionFailure()
{
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

	if (IsValid(TimingGaugeComponent))
	{
		// TimingGaugeComponent->RequestStopGauge();
		// CurrentPlayerController->
	}
}
