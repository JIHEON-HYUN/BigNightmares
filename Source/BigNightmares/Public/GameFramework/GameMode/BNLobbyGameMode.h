// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNLobbyGameMode.generated.h"

USTRUCT()
struct FLobbyPlayerData
{
	GENERATED_BODY()
	
	FString PlayerName;
	bool ReadyState;
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABNLobbyGameMode(const FObjectInitializer& FObjectInitializer= FObjectInitializer::Get());
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	void Ready();
	void UnReady();

private:
	uint32 ReadyCount = 0;
	FTimerHandle GameStartTimer;
	
	void StartGame();
};
