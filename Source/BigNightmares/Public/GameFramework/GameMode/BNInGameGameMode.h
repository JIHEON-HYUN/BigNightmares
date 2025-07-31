// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNInGameGameMode.generated.h"

class UBNMonoCharacterDataAsset;

// 플레이어 직업 enum
UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	None,
	Prey,
	Resident
};

// 플레이어 데이터 구조체 (이름, 직업)
USTRUCT(BlueprintType)
struct FInGamePlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;
	
	UPROPERTY(BlueprintReadWrite)
	EPlayerType PlayerType;

	UPROPERTY(BlueprintReadWrite)
	bool bStatusAlive;

	FInGamePlayerData() : PlayerName(TEXT("")), PlayerType(EPlayerType::None), bStatusAlive(false) {}
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNInGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void PlayerDead();
	
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PrivateMissionEnd();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AssignableMissionEnd();
	
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;
	
private:
	uint8 PlayerCount = 0;
	
	// 캐릭터 기본 베이스 데이터에셋
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
};
