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
		UE_LOG(LogTemp, Error, TEXT("Resident Player is set"));
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

	// Logout을 한 플레이어가 제물 플레이어일 경우, PreyPlayerCount를 감소시켜야 게임 로직에 문제 안 생김
	PlayerDead(GS->GetPlayerType(PS));
	// Logout이 되면 GameState의 InGamePlayerDataList에서 해당 플레이어 삭제
	GS->RemoveLobbyPlayer(PS);
}

void ABNInGameGameMode::PlayerDead(EPlayerType DeadPlayerType)
{
	if (DeadPlayerType == EPlayerType::Prey)
	{
		// d
	}
}

void ABNInGameGameMode::ReturnToLobby()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/L_Lobby?listen");
}

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}


