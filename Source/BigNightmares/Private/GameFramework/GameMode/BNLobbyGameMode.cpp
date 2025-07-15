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
	auto NewPlayerState = NewPlayer->GetPlayerState<ABNPlayerState>();
	auto CurrentGameState = GetGameState<ABNGameState>();
	
	if (NewPlayerState != nullptr)
	{
		NewLobbyPlayer.PlayerName = NewPlayerState->GetPlayerName();
		NewLobbyPlayer.ReadyState = false;
		CurrentGameState->AddLobbyPlayer(NewLobbyPlayer);
	}

	UE_LOG(LogTemp, Warning, TEXT("Name = %s"), *NewLobbyPlayer.PlayerName);
	
	// if (ReadyCount == 12)
	// {
	// 	GetWorldTimerManager().SetTimer(GameStartTimer, this, &ABNLobbyGameMode::StartGame, 30);
	// }
}

void ABNLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	auto ExitPlayerState = Exiting->GetPlayerState<ABNPlayerState>();
	auto CurrentGameState = GetGameState<ABNGameState>();
	
	if (ExitPlayerState != nullptr)
	{
		CurrentGameState->RemoveLobbyPlayer(ExitPlayerState);
	}
}

void ABNLobbyGameMode::Ready()
{
	++ReadyCount;
}

void ABNLobbyGameMode::UnReady()
{
	--ReadyCount;
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
