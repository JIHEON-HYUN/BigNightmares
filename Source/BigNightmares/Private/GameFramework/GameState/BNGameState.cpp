// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameState/BNGameState.h"

#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

#include "GameFramework/GameMode/BNLobbyGameMode.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "Interaction/Mission/MissionTimingGauge.h"
#include "Interaction/Mission/VerticalTimingGaugeComponent.h"


void ABNGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABNGameState, Rep_ActiveGaugeChallenges);
	DOREPLIFETIME(ABNGameState, LobbyPlayerDataList);
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
}

const TArray<FInGamePlayerData>& ABNGameState::GetInGamePlayers() const
{
	return InGamePlayerDataList;
}

void ABNGameState::SetPlayerType(uint8 Index, EPlayerType NewType)
{
	InGamePlayerDataList[Index].PlayerType = NewType;
}

#pragma region Mission1
bool ABNGameState::Server_TryStartSpecificGaugeChallenge(FGuid GaugeID, const ABNPlayerController* PlayerController)
{
	if (!HasAuthority()) return false;
	if (!GaugeID.IsValid()) return false;
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState : Server_TryStartSpecificGaugeChallenge: Requesting BNPlayerController is null."));
		return false; 
	}

	for (const FGaugeChallengeInfo& Info : Rep_ActiveGaugeChallenges)
	{
		if (Info.GaugeID == GaugeID && Info.bIsActive)
			return false;
	}
	
	FGaugeChallengeInfo NewChallengeInfo;
	NewChallengeInfo.GaugeID = GaugeID;
	NewChallengeInfo.ChallengingPlayerState = PlayerController->GetPlayerState<ABNPlayerState>();
	NewChallengeInfo.bIsActive = true;
	Rep_ActiveGaugeChallenges.Add(NewChallengeInfo);		

	// 배열이 변경되었음을 강제로 복제 시스템에 알림 (RepNotify를 트리거)
	// 이 방식은 배열의 원소가 변경될 때 복제를 강제합니다. (ReplicatedUsing과 함께 사용)
	MARK_PROPERTY_DIRTY_FROM_NAME(ABNGameState, Rep_ActiveGaugeChallenges, this);

	UE_LOG(LogTemp, Log, TEXT("Server: Gauge ID '%s' challenge started by Player %s."), *GaugeID.ToString(), *PlayerController->PlayerState->GetPlayerName());
	return true;	
}

void ABNGameState::Server_EndSpecificGaugeChallenge_Implementation(FGuid GaugeID, ABNPlayerController* PlayerController)
{
	if (!HasAuthority()) return;
	if (!GaugeID.IsValid()) return;

	// -1로 초기화.   INDEX_NONE == -1;
	int32 FoundIndex = INDEX_NONE;
	for (int32 i = 0; i < Rep_ActiveGaugeChallenges.Num(); i++)
	{
		if (Rep_ActiveGaugeChallenges[i].GaugeID == GaugeID)
		{
			FoundIndex = i;
			break;
		}
	}

	if (FoundIndex != INDEX_NONE)
	{
		Rep_ActiveGaugeChallenges[FoundIndex].bIsActive = false;
		//TActorIterator 월드 내에 존재하는 특정 타입의 모든 액터들을 순회할 수 있게 함
		for (TActorIterator<AMissionTimingGauge> It(GetWorld()); It; ++It)
		{
			AMissionTimingGauge* MissionGaugeActor = *It;
			UVerticalTimingGaugeComponent* GaugeComponent = MissionGaugeActor->FindComponentByClass<UVerticalTimingGaugeComponent>();
			if (IsValid(GaugeComponent) && GaugeComponent->GaugeID == GaugeID)
			{
				//서버에서 Mission1 로직 비활성화
				GaugeComponent->bIsLogicActive = false;
			}
		}

		MARK_PROPERTY_DIRTY_FROM_NAME(ABNGameState, Rep_ActiveGaugeChallenges, this);
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