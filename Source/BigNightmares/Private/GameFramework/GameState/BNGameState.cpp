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

void ABNGameState::SetPlayerReadyState(const FString& PlayerName, bool NewReadyState)
{
	for (auto& PlayerData : LobbyPlayerDataList)
	{
		if (PlayerData.PlayerName == PlayerName)
		{
			// 레디 상태가 동일하면 처리 X
			if (PlayerData.ReadyState == NewReadyState) return;

			// 레디 상태를 바꾸고, LobbyPlayerDataList 변경 알림
			PlayerData.ReadyState = NewReadyState;
			OnLobbyPlayerUpdated.Broadcast(LobbyPlayerDataList);

			ABNLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ABNLobbyGameMode>();
			if (GM == nullptr) return;
			
			// LobbyGameMode의 레디 상태에 반영
			if (NewReadyState)
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

const TArray<FLobbyPlayerData>& ABNGameState::GetLobbyPlayers() const
{
	return LobbyPlayerDataList;
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
