// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNLobbyGameMode.h"

#include "TimerManager.h"

#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

void ABNLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("로비 포스트 로그인"));
	
	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;

	// PostLogin이 되면 GameState의 LobbyPlayerDataList에 추가
	FLobbyPlayerData NewLobbyPlayer;
	NewLobbyPlayer.PlayerName = PS->GetPlayerName();
	NewLobbyPlayer.bReadyState = false;
	GS->AddLobbyPlayer(NewLobbyPlayer);
}

void ABNLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	auto PS = Exiting->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;

	// PostLogout이 되면 GameState의 InGamePlayerDataList에서 해당 플레이어 삭제
	GS->RemoveLobbyPlayer(PS);
}

void ABNLobbyGameMode::Ready()
{
	++ReadyCount;
	auto GI = GetGameInstance<UBNGameInstance>();
	if (GI == nullptr) return;

	if (ReadyCount == GI->MaxPlayerCount)
	{
		// ReadyCount가 정원이 되면, 게임 시작
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ABNLobbyGameMode::StartGame, 1.0f);
	}
}

void ABNLobbyGameMode::UnReady()
{
	--ReadyCount;
	auto GI = GetGameInstance<UBNGameInstance>();
	if (GI == nullptr) return;

	if (ReadyCount < GI->MaxPlayerCount && GetWorldTimerManager().IsTimerActive(GameStartTimer))
	{
		// 게임 시작 카운트가 시작됐지만 ReadyCount가 줄었으면, Timer 초기화
		GetWorldTimerManager().ClearTimer(GameStartTimer);
	}
}

void ABNLobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UBNGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;

	GameInstance->StartSession();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	// bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Assets/AtmosphericHouse/Maps/L_InGame?listen");
}
