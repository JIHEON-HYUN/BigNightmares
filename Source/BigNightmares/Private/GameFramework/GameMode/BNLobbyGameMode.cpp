// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNLobbyGameMode.h"

void ABNLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumberOfPlayers;

	if (NumberOfPlayers > 2)
	{
		UWorld* World = GetWorld();
		if (World == nullptr) return;
		
		World->ServerTravel("/Game/Levels/L_InGame?listen");
	}
}

void ABNLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--NumberOfPlayers;
}
