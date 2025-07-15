// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameState/BNGameState.h"

#include "Net/UnrealNetwork.h"

#include "GameFramework/GameMode/BNLobbyGameMode.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "UI/Lobby/BNLobbyWidget.h"

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

void ABNGameState::OnRep_LobbyPlayerDataList()
{
	OnLobbyListUpdated.Broadcast(LobbyPlayerDataList);
}

void ABNGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABNGameState, LobbyPlayerDataList);
}
