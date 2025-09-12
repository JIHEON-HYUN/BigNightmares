// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNInGameGameMode.h"

#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

void ABNInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;
	
	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;

	auto GI = GetGameInstance<UBNGameInstance>();
	if (GI == nullptr) return;

	++PlayerCount;

	// PostLogin이 되면 GameState의 InGamePlayerDataList에 추가
	FInGamePlayerData NewInGamePlayer;
	NewInGamePlayer.PlayerName = PS->GetPlayerName();
	NewInGamePlayer.PlayerType = EPlayerType::Prey;
	NewInGamePlayer.bStatusAlive = true;
	GS->AddInGamePlayer(NewInGamePlayer);

	if (PlayerCount == GI->MaxPlayerCount)
	{
		GS->SetPlayerType(FMath::RandRange(0, GS->GetInGamePlayerCount() - 1), EPlayerType::Resident);
	}
}

void ABNInGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	auto PS = Exiting->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	// Logout이 되면 GameState의 InGamePlayerDataList에서 해당 플레이어 삭제
	GS->RemoveInGamePlayer(PS);
}

void ABNInGameGameMode::PlayerDead()
{
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	if (GS->GetPreyPlayerCount() == 0)
	{
		GS->GameEnd(false);
	}
}

void ABNInGameGameMode::ReturnToLobby()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	// bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/L_Lobby?listen");
}

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}
