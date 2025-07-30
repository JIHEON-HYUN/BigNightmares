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

	// PostLogin이 되면 GameState의 InGamePlayerDataList에 추가
	FInGamePlayerData NewInGamePlayer;
	NewInGamePlayer.PlayerName = PS->GetPlayerName();
	NewInGamePlayer.PlayerType = EPlayerType::Prey;
	NewInGamePlayer.bStatusAlive = true;
	GS->AddInGamePlayer(NewInGamePlayer);

	LastPostLoginTime = GetWorld()->GetTimeSeconds();
	if (!GetWorldTimerManager().IsTimerActive(PostLoginTimer))
	{
		GetWorldTimerManager().SetTimer(PostLoginTimer, this, &ABNInGameGameMode::CheckPostLoginTimeOut, 1.0f, true);
	}
}

void ABNInGameGameMode::CheckPostLoginTimeOut()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 3초 동안 PostLogin 없으면
	if (CurrentTime - LastPostLoginTime > 10.f) 
	{
		GetWorldTimerManager().ClearTimer(PostLoginTimer);

		auto GS = GetGameState<ABNGameState>();
		if (GS == nullptr) return;
		
		GS->SetPlayerType(FMath::RandRange(0, GS->GetInGamePlayerCount() - 1), EPlayerType::Resident);
		UE_LOG(LogTemp, Error, TEXT("Resident Player is set"));
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
	GS->RemoveLobbyPlayer(PS);
}

void ABNInGameGameMode::PlayerDead()
{
	UE_LOG(LogTemp, Warning, TEXT("끝끝끝 1"));
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	if (GS->GetPreyPlayerCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("끝끝끝 2"));
		ReturnToLobby();
		GS->GameEnd(false);
		UE_LOG(LogTemp, Warning, TEXT("끝끝끝 3"));
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
