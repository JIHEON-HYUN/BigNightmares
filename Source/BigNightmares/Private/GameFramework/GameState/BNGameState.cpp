// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameState/BNGameState.h"

#include "Net/UnrealNetwork.h"

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

void ABNGameState::SetPlayerReadyState(const FString& PlayerName)
{
	for (auto& PlayerData : LobbyPlayerDataList)
	{
		if (PlayerData.PlayerName == PlayerName)
		{
			// 레디 상태를 바꾸고, LobbyPlayerDataList 변경 알림
			PlayerData.ReadyState = !PlayerData.ReadyState;
			OnLobbyPlayerUpdated.Broadcast(LobbyPlayerDataList);

			ABNLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ABNLobbyGameMode>();
			if (GM == nullptr) return;
			
			// LobbyGameMode의 레디 상태에 반영
			if (PlayerData.ReadyState)
			{
				GM->Ready();
			}
			else
			{
				GM->UnReady();
			}
			
			break;
		}
	}
}

void ABNGameState::OnRep_LobbyPlayerDataList()
{
	OnLobbyPlayerUpdated.Broadcast(LobbyPlayerDataList);
}

void ABNGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABNGameState, LobbyPlayerDataList);
}
