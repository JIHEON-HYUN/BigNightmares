// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNInGameGameMode.h"

#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

void ABNInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	++PlayerCount;

	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;

	// PostLogin이 되면 GameState의 InGamePlayerDataList에 추가
	FInGamePlayerData NewInGamePlayer;
	NewInGamePlayer.PlayerName = PS->GetPlayerName();
	NewInGamePlayer.PlayerType = EPlayerType::Prey;
	NewInGamePlayer.StatusAlive = true;
	GS->AddInGamePlayer(NewInGamePlayer);

	auto GI = GetGameInstance<UBNGameInstance>();
	if (GI == nullptr) return;

	// 현재 플레이어 수가 정원이 되면 랜덤한 1명 거주자 플레이어로 선정
	if (PlayerCount == GI->MaxPlayerCount)
	{
		GS->SetPlayerType(FMath::RandRange(0, PlayerCount - 1), EPlayerType::Resident);
		UE_LOG(LogTemp, Error, TEXT("Resident Player is setted"));

		PreyPlayerCount = PlayerCount - 1;
	}
}

void ABNInGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	--PlayerCount;

	auto PS = Exiting->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	// PostLogout이 되면 GameState의 InGamePlayerDataList에서 해당 플레이어 삭제
	GS->RemoveLobbyPlayer(PS);
}

void ABNInGameGameMode::PlayerDead(EPlayerType DeadPlayerType)
{
	if (DeadPlayerType == EPlayerType::Prey)
	{
		--PreyPlayerCount;
	}

	// 제물 플레이어가 0명이 되면 제물 플레이어 패배, 로비로 이동
	if (PreyPlayerCount == 0)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
	
		bUseSeamlessTravel = true;
		World->ServerTravel("/Game/Levels/L_Lobby?listen");
	}
}

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}


