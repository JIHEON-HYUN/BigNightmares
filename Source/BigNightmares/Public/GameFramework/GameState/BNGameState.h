// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BNGameState.generated.h"

struct FLobbyPlayerData;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void AddLobbyPlayer(const FLobbyPlayerData& NewPlayer);
	void RemoveLobbyPlayer(class ABNPlayerState* ExitPlayerState);
	
	const TArray<FLobbyPlayerData>& GetLobbyPlayers() const;
	
private:
	TArray<FLobbyPlayerData> LobbyPlayerDataList;
};
