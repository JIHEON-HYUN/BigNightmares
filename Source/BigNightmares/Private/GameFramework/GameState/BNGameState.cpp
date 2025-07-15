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
	// 클라이언트로 동기화되었을 때 UI 갱신

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	APlayerController* LocalPC = World->GetFirstPlayerController();
	if (LocalPC == nullptr || !LocalPC->IsLocalController()) return;

	// 위젯 참조 시도
	auto BNController = Cast<ABNPlayerController>(LocalPC);
	if (BNController && BNController->LobbyWidget)
	{
		// GameState에서 현재 동기화된 리스트 전달
		BNController->LobbyWidget->SetPlayerList(LobbyPlayerDataList);
	}
}

void ABNGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABNGameState, LobbyPlayerDataList);
}
