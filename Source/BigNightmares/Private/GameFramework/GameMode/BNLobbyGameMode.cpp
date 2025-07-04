// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNLobbyGameMode.h"

#include "TimerManager.h"

#include "GameFramework/GameInstance/BNGameInstance.h"

void ABNLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumberOfPlayers;

	if (NumberOfPlayers > 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ABNLobbyGameMode::StartGame, 30);
	}
}

void ABNLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--NumberOfPlayers;
}

void ABNLobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UBNGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;

	GameInstance->StartSession();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/L_InGame?listen");
}
