// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNCoopMissionGameMode.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h" 

ABNCoopMissionGameMode::ABNCoopMissionGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABNCoopMissionGameMode::MissionComplete()
{
	if (bMissionIsComplete)
	{
		return;
	}
	bMissionIsComplete = true;

	Multicast_ShowClearUI();
}

void ABNCoopMissionGameMode::AllPlayersKilledByThinman(AActor* Killer)
{
    UE_LOG(LogTemp, Warning, TEXT("--- [GameMode] AllPlayersKilledByThinman CALLED by %s ---"), *Killer->GetName());

    if (bIsGameOver)
    {
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] SKIPPED: Game is already over."));
        return;
    }
    bIsGameOver = true;

	UE_LOG(LogTemp, Warning, TEXT("[GameMode] Found %d logged in players. Attempting to kill all."), LoggedInPlayers.Num());

	for (APlayerController* PlayerController : LoggedInPlayers)
	{
		if (PlayerController)
		{
			ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(PlayerController->GetPawn());
			if (PlayerCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("[GameMode] -> Killing player: %s"), *PlayerCharacter->GetName());
				PlayerCharacter->HandleImmediateDeath(Killer);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[GameMode] -> FAILED to get PlayerCharacter from Controller %s"), *PlayerController->GetName());
			}
		}
	}
}


void ABNCoopMissionGameMode::Multicast_ShowClearUI_Implementation()
{
	if (MissionClearWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionClearWidgetClass is not set in GameMode."));
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->IsLocalController()) 
		{
			UUserWidget* ClearWidget = CreateWidget<UUserWidget>(PC, MissionClearWidgetClass);
			if (ClearWidget)
			{
				ClearWidget->AddToViewport();
			}
		}
	}
}

void ABNCoopMissionGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
	{
		return;
	}

	LoggedInPlayers.AddUnique(NewPlayer);
	
	if (LoggedInPlayers.Num() > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("A third player tried to join. The game is full."));
	}
}

void ABNCoopMissionGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LoggedInPlayers.Num() != 2 || bIsGameOver)
	{
		return;
	}

	APlayerController* Player1Controller = LoggedInPlayers[0];
	APlayerController* Player2Controller = LoggedInPlayers[1];

	if (Player1Controller && Player2Controller)
	{
		ABNMonoCharacter* Player1Character = Cast<ABNMonoCharacter>(Player1Controller->GetPawn());
		ABNMonoCharacter* Player2Character = Cast<ABNMonoCharacter>(Player2Controller->GetPawn());

		if (Player1Character && Player2Character && 
			Player1Character->PlayerRole == EPlayerRole::None && 
			Player2Character->PlayerRole == EPlayerRole::None)
		{
			UE_LOG(LogTemp, Error, TEXT("======== Pawns are ready and have no roles! Assigning roles now. This will only happen ONCE. ========"));

			if (FMath::RandBool())
			{
				Player1Character->PlayerRole = EPlayerRole::FlashlightHolder;
				Player2Character->PlayerRole = EPlayerRole::KeyHolder;
			}
			else
			{
				Player1Character->PlayerRole = EPlayerRole::KeyHolder;
				Player2Character->PlayerRole = EPlayerRole::FlashlightHolder;
			}
		}
	}
}
