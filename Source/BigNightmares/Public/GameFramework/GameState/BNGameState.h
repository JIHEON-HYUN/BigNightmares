// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BNGameState.generated.h"

struct FLobbyPlayerData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyPlayerUpdated, const TArray<FLobbyPlayerData>&, NewList);

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
	void SetPlayerReadyState(const FString& PlayerName, bool NewReadyState);
	const TArray<FLobbyPlayerData>& GetLobbyPlayers() const;

	UPROPERTY(BlueprintAssignable)
	FLobbyPlayerUpdated OnLobbyPlayerUpdated;
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_LobbyPlayerDataList)
	TArray<FLobbyPlayerData> LobbyPlayerDataList;

	UFUNCTION()
	void OnRep_LobbyPlayerDataList();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
