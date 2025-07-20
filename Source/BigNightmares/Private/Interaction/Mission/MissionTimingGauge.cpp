// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/MissionTimingGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

#include "Interaction/Mission/VerticalTimingGaugeComponent.h"

AMissionTimingGauge::AMissionTimingGauge()
{
	TimingGaugeComponent = CreateDefaultSubobject<UVerticalTimingGaugeComponent>(TEXT("VerticalTimingGauge"));
	AddOwnedComponent(TimingGaugeComponent);
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
		TimingGaugeComponent->OnGaugeFinished.AddDynamic(this, &AMissionTimingGauge::OnGaugeFinished);
		if (!TimingGaugeComponent->GaugeID.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("AMissionTimingGauge '%s': TimingGaugeComponent's GaugeID is not set! Please set a unique ID in the editor."), *GetName());
			TimingGaugeComponent->GaugeID = FGuid::NewGuid();
		}
	}
}

void AMissionTimingGauge::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor)
	{
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);
		if (IsValid(OverlappingPawn) && OverlappingPawn->GetPlayerState())
		{
			ABNPlayerController* BNPC = Cast<ABNPlayerController>(OverlappingPawn->GetController());
			if (IsValid(BNPC))
			{
				UE_LOG(LogTemp, Warning, TEXT("Server: Overlapping Pawn's Controller is a valid ABNPlayerController. OtherActor: %s"), *OtherActor->GetName());
				ABNPlayerState* BNPS = Cast<ABNPlayerState>(OverlappingPawn->GetPlayerState());
				if (IsValid(BNPS))
				{
					ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
					if (IsValid(BNGS) && IsValid(TimingGaugeComponent))
					{
						if (BNGS->Server_TryStartSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, BNPC))
						{
							CurrentChallengingPlayerState = Cast<ABNPlayerState>(BNPS);
							UE_LOG(LogTemp, Log, TEXT("Server: Player %s started challenge for Gauge ID %s"), *BNPS->GetPlayerName(), *TimingGaugeComponent->GaugeID.ToString());

							TimingGaugeComponent->RequestStartGauge(BNPC);
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Server: Gauge ID %s is already active or cannot be started (by %s)."), *TimingGaugeComponent->GaugeID.ToString(), *BNPS->GetPlayerName());
						}
					}
				}				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Server: Overlapping Pawn's Controller is not a valid ABNPlayerController or is null. OtherActor: %s"), *OtherActor->GetName());
			}
		}
	}
}

void AMissionTimingGauge::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor)
	{
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);
		if (OverlappingPawn && OverlappingPawn->GetPlayerState())
		{
			ABNPlayerState* BNPS = Cast<ABNPlayerState>(OverlappingPawn->GetPlayerState());
			if (IsValid(CurrentChallengingPlayerState) && CurrentChallengingPlayerState == BNPS)
			{
				ABNPlayerController* BNPC = Cast<ABNPlayerController>(OverlappingPawn->GetController());
				if (BNPC)
				{
					ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
					if (BNGS && IsValid(TimingGaugeComponent))
					{
						BNGS->Server_EndSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, BNPC);
					} 
					if (IsValid(TimingGaugeComponent))
					{
						TimingGaugeComponent->Client_EndGaugeUI(EVerticalGaugeResult::EVGR_Fail);
					}
					UE_LOG(LogTemp, Log, TEXT("Server: Player %s left overlap, challenge for Gauge ID %s forcefully ended."), *CurrentChallengingPlayerState->GetPlayerName(), *TimingGaugeComponent->GaugeID.ToString());
					CurrentChallengingPlayerState = nullptr;
				}
			}
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

		if (IsValid(CurrentChallengingPlayerState))
		{
			ABNPlayerController* BNPC = Cast<ABNPlayerController>(CurrentChallengingPlayerState->GetPlayerController());
			if (BNPC)
			{
				ABNGameState* BNGS = GetWorld()->GetGameState<ABNGameState>();
				if (BNGS && IsValid(TimingGaugeComponent))
				{
					BNGS->Server_EndSpecificGaugeChallenge(TimingGaugeComponent->GaugeID, BNPC);
				}
			}

			if (Result == EVerticalGaugeResult::EVGR_Success)
			{
				if (GetOverlapEffect() && IsValid(CurrentChallengingPlayerState) &&  IsValid(CurrentChallengingPlayerState->GetPawn()))
				{
					if (UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CurrentChallengingPlayerState->GetPawn()))
					{
						const FGameplayEffectContextHandle ContextHandle = OtherASC->MakeEffectContext();
						const FGameplayEffectSpecHandle SpecHandle = OtherASC->MakeOutgoingSpec(GetOverlapEffect(), 1.f, ContextHandle);
						OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
						UE_LOG(LogTemp, Log, TEXT("Server: Applied GameplayEffect for success to player %s."), *CurrentChallengingPlayerState->GetPlayerName());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Server: Failed to get AbilitySystemComponent for player %s."), *CurrentChallengingPlayerState->GetPlayerName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Server: OverlapEffect is not set or ChallengingPlayerState/Pawn is invalid for success."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Server: Mission failed for Gauge ID '%s' by player %s."), *TimingGaugeComponent->GaugeID.ToString(), *CurrentChallengingPlayerState->GetPlayerName());
			}
			CurrentChallengingPlayerState = nullptr;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: OnGaugeFinished called but no valid challenging player state was set."));
		}

		Destroy();
	}
}
