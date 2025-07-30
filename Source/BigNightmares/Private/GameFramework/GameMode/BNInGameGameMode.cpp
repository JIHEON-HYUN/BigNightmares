// BNInGameGameMode.cpp

#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "GameFramework/GameInstance/BNGameInstance.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABNInGameGameMode::ABNInGameGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABNInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer == nullptr) return;

	// 메인 게임 플레이어 입장 로직만 처리
	auto PS = NewPlayer->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;

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

void ABNInGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting == nullptr) return;

	// 메인 게임 플레이어 퇴장 로직만 처리
	auto PS = Exiting->GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	GS->RemoveLobbyPlayer(PS);
}

void ABNInGameGameMode::CheckPostLoginTimeOut()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastPostLoginTime > 3.f) 
	{
		GetWorldTimerManager().ClearTimer(PostLoginTimer);
		auto GS = GetGameState<ABNGameState>();
		if (GS == nullptr) return;

		// 플레이어 중 한 명에게 'Resident' 역할을 무작위로 배정
		GS->SetPlayerType(FMath::RandRange(0, GS->GetInGamePlayerCount() - 1), EPlayerType::Resident);
	}
}

void ABNInGameGameMode::PlayerDead()
{
	auto GS = GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	if (GS->GetPreyPlayerCount() == 0)
	{
		ReturnToLobby();
		GS->GameEnd(false);
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
