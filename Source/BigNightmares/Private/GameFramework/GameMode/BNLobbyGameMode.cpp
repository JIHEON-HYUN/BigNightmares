// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNLobbyGameMode.h"

#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

ABNLobbyGameMode::ABNLobbyGameMode(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
}

void ABNLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	FLobbyPlayerData NewLobbyPlayer;
	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	NewLobbyPlayer.PlayerName = PS->GetPlayerName();
	NewLobbyPlayer.ReadyState = false;
	GS->AddLobbyPlayer(NewLobbyPlayer);
}

void ABNLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	auto PS = Exiting->GetPlayerState<ABNPlayerState>();
	auto GS = GetGameState<ABNGameState>();
	
	if (PS != nullptr)
	{
		GS->RemoveLobbyPlayer(PS);
	}
}

void ABNLobbyGameMode::SetMaxReadyCount(uint8 NewMaxReadyCount)
{
	MaxReadyCount = NewMaxReadyCount;
}

void ABNLobbyGameMode::Ready()
{
	++ReadyCount;

	if (ReadyCount == MaxReadyCount)
	{
		// ReadyCount가 정원이 되면, 3초 후에 게임 시작
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ABNLobbyGameMode::StartGame, 1);
	}
}

void ABNLobbyGameMode::UnReady()
{
	--ReadyCount;

	if (ReadyCount < MaxReadyCount && GetWorldTimerManager().IsTimerActive(GameStartTimer))
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
	
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/L_InGame?listen");
}
