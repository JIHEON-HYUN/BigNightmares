// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	uint32 NumberOfPlayers = 0;
	FTimerHandle GameStartTimer;
	
	void StartGame();
};
