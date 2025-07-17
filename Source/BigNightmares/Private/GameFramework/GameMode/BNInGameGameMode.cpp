// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNInGameGameMode.h"

#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

void ABNInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	FInGamePlayerData NewInGamePlayer;
	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	NewInGamePlayer.PlayerName = PS->GetPlayerName();
	NewInGamePlayer.PlayerType = EPlayerType::Prey;
	GS->AddInGamePlayer(NewInGamePlayer);
}

void ABNInGameGameMode::Logout(AController* Exiting)
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

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}

