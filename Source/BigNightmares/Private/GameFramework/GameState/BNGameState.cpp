// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameState/BNGameState.h"

#include "GameFramework/GameMode/BNLobbyGameMode.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "Net/UnrealNetwork.h"


void ABNGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABNGameState, Rep_ActiveGaugeChallenges);
	DOREPLIFETIME(ABNGameState, LobbyPlayerDataList);
	DOREPLIFETIME(ABNGameState, InGamePlayerDataList);
	DOREPLIFETIME(ABNGameState, PreyPlayerCount);
}

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
			PlayerData.bReadyState = !PlayerData.bReadyState;
			OnLobbyPlayerUpdated.Broadcast(LobbyPlayerDataList);

			ABNLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ABNLobbyGameMode>();
			if (GM == nullptr) return;
			
			// LobbyGameMode의 레디 상태에 반영
			if (PlayerData.bReadyState)
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

void ABNGameState::AddInGamePlayer(const FInGamePlayerData& NewPlayer)
{
	InGamePlayerDataList.Add(NewPlayer);
}

void ABNGameState::RemoveInGamePlayer(class ABNPlayerState* ExitPlayerState)
{
	InGamePlayerDataList.RemoveAll([ExitPlayerState](const FInGamePlayerData& RemovePlayerData)
		{
			return RemovePlayerData.PlayerName == ExitPlayerState->GetPlayerName();
		});
	
	if (GetPlayerType(ExitPlayerState) == EPlayerType::Prey)
	{
		-- PreyPlayerCount;
	}
}

const TArray<FInGamePlayerData>& ABNGameState::GetInGamePlayers() const
{
	return InGamePlayerDataList;
}

const EPlayerType ABNGameState::GetPlayerType(ABNPlayerState* PlayerState) const
{
	for (auto PlayerData : InGamePlayerDataList)
	{
		if (PlayerData.PlayerName == PlayerState->GetPlayerName())
		{
			return PlayerData.PlayerType;
		}
	}

	return EPlayerType::None;
}

uint8 ABNGameState::GetInGamePlayerCount() const
{
	return InGamePlayerDataList.Num();
}

uint8 ABNGameState::GetPreyPlayerCount() const
{
	return PreyPlayerCount;
}

void ABNGameState::SetPlayerType(uint8 Index, EPlayerType NewType)
{
	InGamePlayerDataList[Index].PlayerType = NewType;
	if (NewType == EPlayerType::Resident)
	{
		PreyPlayerCount = GetInGamePlayerCount() - 1;
	}

	for (const auto& PlayerData : InGamePlayerDataList)
	{
		UE_LOG(LogTemp, Error, TEXT("Prey Count : %d"), GetPreyPlayerCount());
		UE_LOG(LogTemp, Error, TEXT("Player Name : %s"), *PlayerData.PlayerName);
		if (PlayerData.PlayerType == EPlayerType::Resident)
		{
			UE_LOG(LogTemp, Error, TEXT("Player Type : Resident"));
		}
		else if (PlayerData.PlayerType == EPlayerType::Prey)
		{
			UE_LOG(LogTemp, Error, TEXT("Player Type : Prey"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player Type : None"));
		}
	}
}

void ABNGameState::SetPlayerStatusAlive(const FString& PlayerName)
{
	for (auto& PlayerData : InGamePlayerDataList)
	{
		if (PlayerData.PlayerName == PlayerName)
		{
			// 생존 상태를 바꾸고, InGamePlayerDataList 변경 알림
			PlayerData.bStatusAlive = !PlayerData.bStatusAlive;
			OnInGamePlayerUpdated.Broadcast(InGamePlayerDataList);

			ABNInGameGameMode* GM = GetWorld()->GetAuthGameMode<ABNInGameGameMode>();
			if (GM == nullptr) return;

			if (PlayerData.PlayerType == EPlayerType::Prey)
			{
				--PreyPlayerCount;
			}
			
			if (!PlayerData.bStatusAlive)
			{
				GM->PlayerDead();
			}
			
			break;
		}
	}
}

void ABNGameState::OnRep_InGamePlayerDataList()
{
	OnInGamePlayerUpdated.Broadcast(InGamePlayerDataList);
}

#pragma region Mission1

bool ABNGameState::Server_TryStartSpecificGaugeChallenge(FGuid GaugeID, const ABNPlayerController* PlayerController)
{
	if (!HasAuthority()) return false;
	if (!PlayerController || !PlayerController->PlayerState) return false;

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState : Server_RequestStartGaugeInternal: Requesting BNPlayerController is null."));
		return false; 
	}

	FGaugeChallengeInfo* FoundInfo = Rep_ActiveGaugeChallenges.FindByPredicate([&] (const FGaugeChallengeInfo& Info)
	{
		return Info.GaugeID == GaugeID;
	});

	if (FoundInfo && FoundInfo->bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Gauge ID '%s' is already active."), *GaugeID.ToString());
		return false;
	}

	//다른 플레이어가 도전 중인지 확인
	for (const FGaugeChallengeInfo& Info : Rep_ActiveGaugeChallenges)
	{
		if (Info.bIsActive && Info.ChallengingPlayerState == PlayerController->PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Player %s is already challenging another gauge (ID: %s)."), *PlayerController->PlayerState->GetPlayerName(), *Info.GaugeID.ToString());
			return false;			
		}
	}

	// 업데이트
	if (FoundInfo)
	{
		FoundInfo->ChallengingPlayerState = Cast<ABNPlayerState>(PlayerController->PlayerState);
		FoundInfo->bIsActive = true;
	}
	else
	{
		FGaugeChallengeInfo NewChallengeInfo;
		NewChallengeInfo.GaugeID = GaugeID;
		NewChallengeInfo.ChallengingPlayerState = Cast<ABNPlayerState>(PlayerController->PlayerState);
		NewChallengeInfo.bIsActive = true;
		Rep_ActiveGaugeChallenges.Add(NewChallengeInfo);		
	}

	// 배열이 변경되었음을 강제로 복제 시스템에 알림 (RepNotify를 트리거)
	// 이 방식은 배열의 원소가 변경될 때 복제를 강제합니다. (ReplicatedUsing과 함께 사용)
	MARK_PROPERTY_DIRTY_FROM_NAME(ABNGameState, Rep_ActiveGaugeChallenges, this);

	UE_LOG(LogTemp, Log, TEXT("Server: Gauge ID '%s' challenge started by Player %s."), *GaugeID.ToString(), *PlayerController->PlayerState->GetPlayerName());
	return true;	
}

void ABNGameState::Server_EndSpecificGaugeChallenge(FGuid GaugeID, ABNPlayerController* PlayerController)
{
	if (!HasAuthority()) return;

	FGaugeChallengeInfo* FoundInfo = Rep_ActiveGaugeChallenges.FindByPredicate([&](const FGaugeChallengeInfo& Info)
	{
		return Info.GaugeID == GaugeID;
	});

	if (FoundInfo && FoundInfo->bIsActive)
	{
		FoundInfo->bIsActive = false;
		FoundInfo->ChallengingPlayerState = nullptr; //도전 종료시 플레이어 초기화

		//복제 시스템 변경 알림
		MARK_PROPERTY_DIRTY_FROM_NAME(ABNGameState, Rep_ActiveGaugeChallenges, this);
		UE_LOG(LogTemp, Log, TEXT("Server: Gauge ID '%s' challenge ended."), *GaugeID.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Attempted to end inactive or non-existent gauge ID '%s'."), *GaugeID.ToString());
	}
}

void ABNGameState::OnRep_ActiveGaugeChallenges()
{
	Client_ActiveGaugeChallengesMap.Empty();
	for (const FGaugeChallengeInfo& Info : Rep_ActiveGaugeChallenges)
	{
		if (IsValid(Info.ChallengingPlayerState))
		{
			Client_ActiveGaugeChallengesMap.Add(Info.GaugeID, Info); //맵에 추가

			// 누가 도전 중인지 확인
			UE_LOG(LogTemp, Log, TEXT("Client: Gauge %s is active, challenged by Player: %s"),
				*Info.GaugeID.ToString(), *Info.ChallengingPlayerState->GetPlayerName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Client: Gauge %s has an invalid ChallengingPlayerState."), *Info.GaugeID.ToString());
		}
	}

	// 클라에서 필요한 추가 로직 수행가능 ex) UI업데이트, 게이지 상태 시각화등
	UE_LOG(LogTemp, Log, TEXT("ActiveGaugeChallengesArray replicated. Client Map updated with %d entries."), Client_ActiveGaugeChallengesMap.Num());
}

bool ABNGameState::IsGaugeChallengeActive(FGuid GaugeID) const
{
	if (GetLocalRole() == ROLE_Authority) // 서버라면 직접 배열에서 확인
	{
		const FGaugeChallengeInfo* FoundInfo = Rep_ActiveGaugeChallenges.FindByPredicate([&] (const FGaugeChallengeInfo& Info)
		{
			return Info.GaugeID == GaugeID;
		});
		return FoundInfo && FoundInfo->bIsActive;
	}
	else //클라이언트라면 복제된 맵에서 확인
	{
		const FGaugeChallengeInfo* FoundInfo = Client_ActiveGaugeChallengesMap.Find(GaugeID);
		return FoundInfo && FoundInfo->bIsActive;
	}
}

ABNPlayerState* ABNGameState::GetChallengingPlayerStateForGauge(FGuid GaugeID) const
{
	if (GetLocalRole() == ROLE_Authority) //서버라면 직접 가져가기
	{
		const FGaugeChallengeInfo* FoundInfo = Rep_ActiveGaugeChallenges.FindByPredicate([&](const FGaugeChallengeInfo& Info)
		{
			return Info.GaugeID == GaugeID;
		});
		return FoundInfo ? FoundInfo->ChallengingPlayerState : nullptr;
	}
	else //클라면 복제된 맵에서
	{
		const FGaugeChallengeInfo* FoundInfo = Client_ActiveGaugeChallengesMap.Find(GaugeID);
		return FoundInfo ? FoundInfo->ChallengingPlayerState : nullptr;
	}
}
#pragma endregion