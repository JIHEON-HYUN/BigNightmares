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
	
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;
	
private:
	uint8 PlayerCount = 0;
	
	// 서버에서만 처리해야할 데이터는 gamemode에 선언되어야 함. 해당 변수는 캐릭터의 기본 베이스 데이터에셋임.
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
};
