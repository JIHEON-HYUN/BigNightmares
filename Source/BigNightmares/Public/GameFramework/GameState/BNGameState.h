// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BNGameState.generated.h"

enum class EPlayerType : uint8;

class ABNPlayerController;
class ABNPlayerState;
struct FLobbyPlayerData;
struct FInGamePlayerData;

USTRUCT()
struct FGaugeChallengeInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid GaugeID;

	// 현재 도전중인 플리에어
	UPROPERTY()
	TObjectPtr<ABNPlayerState> ChallengingPlayerState;

	UPROPERTY()
	bool bIsActive;

	FGaugeChallengeInfo() : bIsActive(false) {}
};

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

#pragma endregion InGame
	
#pragma region Mission1
public:

	//서버 : 특정 게이지 미션 도전 시작
	bool Server_TryStartSpecificGaugeChallenge(FGuid GaugeID, const ABNPlayerController* PlayerController);

	//서버 : 특정 게이지 미션 도전 종료
	void Server_EndSpecificGaugeChallenge(FGuid GaugeID, ABNPlayerController* PlayerController);

protected:
	UPROPERTY(ReplicatedUsing=OnRep_ActiveGaugeChallenges)
	TArray<FGaugeChallengeInfo> Rep_ActiveGaugeChallenges;

	//Rep_ActiveGaugeChallenges 복제 될 때 불릴 함수
	UFUNCTION()
	void OnRep_ActiveGaugeChallenges();

	//클라이언트에서 사용될 복제된 정보 기반의 맵 (맵은 Replicated가 안됨)
	TMap<FGuid, FGaugeChallengeInfo> Client_ActiveGaugeChallengesMap;

	//클라에서 특정 게이지의 도전 상태를 조회
	UFUNCTION(BlueprintCallable,Category="Timing Gauge | GameState")
	bool IsGaugeChallengeActive(FGuid GaugeID) const;

	UFUNCTION(BlueprintCallable,Category="Timing Gauge | GameState")
	ABNPlayerState* GetChallengingPlayerStateForGauge(FGuid GaugeID) const;
	
#pragma endregion
};
