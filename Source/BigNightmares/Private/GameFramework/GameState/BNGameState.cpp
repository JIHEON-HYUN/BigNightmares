// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameState/BNGameState.h"

#include "GameFramework/GameMode/BNLobbyGameMode.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

void ABNGameState::AddLobbyPlayer(const FLobbyPlayerData& NewPlayer)
{
	LobbyPlayerDataList.Add(NewPlayer);
}

void ABNGameState::RemoveLobbyPlayer(ABNPlayerState* ExitPlayerState)
{
	LobbyPlayerDataList.RemoveAll([ExitPlayerState](const FLobbyPlayerData& RemovePlayerData)
		{
			return RemovePlayerData.PlayerName == ExitPlayerState->GetPlayerName();
		});
}

const TArray<FLobbyPlayerData>& ABNGameState::GetLobbyPlayers() const
{
	return LobbyPlayerDataList;
}
