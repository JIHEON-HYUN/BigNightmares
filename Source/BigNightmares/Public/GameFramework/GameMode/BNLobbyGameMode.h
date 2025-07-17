// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNLobbyGameMode.generated.h"

// 플레이어 데이터 구조체 (이름, 준비상태)
USTRUCT(BlueprintType)
struct FLobbyPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;
	
	UPROPERTY(BlueprintReadWrite)
	bool ReadyState = false;
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
	uint8 ReadyCount = 0;
	FTimerHandle GameStartTimer;
	
	void StartGame();
};
