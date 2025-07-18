// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BNGameState.generated.h"

enum class EPlayerType : uint8;

struct FLobbyPlayerData;
struct FInGamePlayerData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyPlayerUpdated, const TArray<FLobbyPlayerData>&, NewList);

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Lobby
	
public:
	void AddLobbyPlayer(const FLobbyPlayerData& NewPlayer);
	void RemoveLobbyPlayer(class ABNPlayerState* ExitPlayerState);
	const TArray<FLobbyPlayerData>& GetLobbyPlayers() const;
	void SetPlayerReadyState(const FString& PlayerName);

	UPROPERTY(BlueprintAssignable)
	FLobbyPlayerUpdated OnLobbyPlayerUpdated;
	
protected:
	UPROPERTY(ReplicatedUsing = OnRep_LobbyPlayerDataList)
	TArray<FLobbyPlayerData> LobbyPlayerDataList;

	UFUNCTION()
	void OnRep_LobbyPlayerDataList();
	
#pragma endregion Lobby

#pragma region InGame

public:
	void AddInGamePlayer(const FInGamePlayerData& NewPlayer);
	void RemoveInGamePlayer(class ABNPlayerState* ExitPlayerState);
	const TArray<FInGamePlayerData>& GetInGamePlayers() const;
	void SetPlayerType(uint8 Index, EPlayerType NewType);

protected:
	TArray<FInGamePlayerData> InGamePlayerDataList;

public:
	// uint8 RemainingPrivateMissionCount = 1;
	// uint8 RemainingCooperativeMissionCount = 1;
	// uint8 RemainingSacrificeMissionCount = 1;
	// uint8 RemainingAssignableMissionCount = 1;
	//
	// void ClearPrivateMission();
	// void ClearCooperativeMission();
	// void ClearSacrificeMission();
	// void ClearAssignableMission();
	
#pragma endregion InGame
};
