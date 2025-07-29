// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

ABNInGameGameMode::ABNInGameGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	RandomStream.GenerateNewSeed();
}

void ABNInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	// [핵심] 현재 게임 모드 유형에 따라 다른 로직을 실행합니다.
	if (CurrentGameModeType == EGameModeType::CoopMission)
	{
		Coop_LoggedInPlayers.AddUnique(NewPlayer);
		
		if (Coop_LoggedInPlayers.Num() == 2 && !bRolesAssigned)
		{
			GetWorldTimerManager().SetTimer(RoleAssignmentTimer, this, &ABNInGameGameMode::AttemptToAssignCoopRoles, 1.0f, true);
		}
	}
	else // MainGame 모드일 경우
	{
		auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
		if (PS == nullptr) return;
		auto GS = GetGameState<ABNGameState>();
		if (GS == nullptr) return;

		FInGamePlayerData NewInGamePlayer;
		NewInGamePlayer.PlayerName = PS->GetPlayerName();
		NewInGamePlayer.PlayerType = EPlayerType::Prey;
		NewInGamePlayer.bStatusAlive = true;
		GS->AddInGamePlayer(NewInGamePlayer);

		LastPostLoginTime = GetWorld()->GetTimeSeconds();
		if (!GetWorldTimerManager().IsTimerActive(PostLoginTimer))
		{
			GetWorldTimerManager().SetTimer(PostLoginTimer, this, &ABNInGameGameMode::CheckPostLoginTimeOut, 1.0f, true);
		}
	}
}

void ABNInGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	// [핵심] 현재 게임 모드 유형에 따라 다른 로직을 실행합니다.
	if (CurrentGameModeType == EGameModeType::CoopMission)
	{
		APlayerController* ExitingPlayerController = Cast<APlayerController>(Exiting);
		if(ExitingPlayerController)
		{
			Coop_LoggedInPlayers.Remove(ExitingPlayerController);
		}
		GetWorldTimerManager().ClearTimer(RoleAssignmentTimer);
	}
	else // MainGame 모드일 경우
	{
		auto PS = Exiting->GetPlayerState<ABNPlayerState>();
		if (PS == nullptr) return;
		auto GS = GetGameState<ABNGameState>();
		if (GS == nullptr) return;
		GS->RemoveLobbyPlayer(PS);
	}
}

// --- 메인 게임 전용 로직 ---
void ABNInGameGameMode::CheckPostLoginTimeOut()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastPostLoginTime > 3.f) 
	{
		GetWorldTimerManager().ClearTimer(PostLoginTimer);
		auto GS = GetGameState<ABNGameState>();
		if (GS == nullptr) return;
		GS->SetPlayerType(FMath::RandRange(0, GS->GetInGamePlayerCount() - 1), EPlayerType::Resident);
	}
}

// --- 협동 미션 전용 로직 ---
void ABNInGameGameMode::AttemptToAssignCoopRoles()
{
    if (bRolesAssigned || Coop_LoggedInPlayers.Num() != 2)
    {
		GetWorldTimerManager().ClearTimer(RoleAssignmentTimer);
        return;
    }

    APlayerController* Player1Controller = Coop_LoggedInPlayers[0];
    APlayerController* Player2Controller = Coop_LoggedInPlayers[1];

    if (Player1Controller && Player2Controller)
    {
        ABNMonoCharacter* Player1Character = Cast<ABNMonoCharacter>(Player1Controller->GetPawn());
        ABNMonoCharacter* Player2Character = Cast<ABNMonoCharacter>(Player2Controller->GetPawn());

        if (Player1Character && Player2Character)
        {
            if (RandomStream.RandRange(0, 1) == 1) // [최종 수정] 올바른 함수인 RandRange를 사용합니다.
            {
                Player1Character->PlayerRole = EPlayerRole::FlashlightHolder;
                Player2Character->PlayerRole = EPlayerRole::KeyHolder;
            }
            else
            {
                Player1Character->PlayerRole = EPlayerRole::KeyHolder;
                Player2Character->PlayerRole = EPlayerRole::FlashlightHolder;
            }
            
            bRolesAssigned = true;
			GetWorldTimerManager().ClearTimer(RoleAssignmentTimer);

            UE_LOG(LogTemp, Warning, TEXT("Co-op roles assigned -> P1: %s, P2: %s"), 
                *UEnum::GetValueAsString(Player1Character->PlayerRole), 
                *UEnum::GetValueAsString(Player2Character->PlayerRole));
        }
    }
}

void ABNInGameGameMode::MissionComplete()
{
    if (bMissionIsComplete) return;
    bMissionIsComplete = true;
    Multicast_ShowClearUI();
}

void ABNInGameGameMode::AllPlayersKilledByThinman(AActor* Killer)
{
    if (bIsGameOver) return;
    bIsGameOver = true;

    for (APlayerController* PlayerController : Coop_LoggedInPlayers)
    {
        if (PlayerController)
        {
            ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(PlayerController->GetPawn());
            if (PlayerCharacter)
            {
                PlayerCharacter->HandleImmediateDeath(Killer);
            }
        }
    }
}

void ABNInGameGameMode::Multicast_ShowClearUI_Implementation()
{
    if (MissionClearWidgetClass == nullptr) return;

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

// --- 공용 로직 ---
void ABNInGameGameMode::PlayerDead()
{
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	if (GS->GetPreyPlayerCount() == 0)
	{
		ReturnToLobby();
		GS->GameEnd(false);
	}
}

void ABNInGameGameMode::ReturnToLobby()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/L_Lobby?listen");
}

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}
