// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Math/RandomStream.h"
#include "BNInGameGameMode.generated.h"

class UBNMonoCharacterDataAsset;
class UUserWidget;

// [추가] 현재 게임 모드가 메인 게임인지, 협동 미션인지 구분하기 위한 Enum
UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	MainGame,
	CoopMission
};

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


UCLASS()
class BIGNIGHTMARES_API ABNInGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ABNInGameGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
    
	void PlayerDead();
	
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;
    
    // --- 협동 미션 기능 ---
    void MissionComplete();
    void AllPlayersKilledByThinman(AActor* Killer);
    
    UFUNCTION(BlueprintPure, Category = "Coop Mission")
    bool IsGameOver() const { return bIsGameOver; }
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Coop Mission|UI")
    TSubclassOf<UUserWidget> MissionClearWidgetClass;

	// [추가] 에디터에서 현재 게임 모드의 유형을 설정할 수 있는 변수입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Mode Settings")
	EGameModeType CurrentGameModeType = EGameModeType::MainGame;

private:
	// 메인 게임용 타이머 및 변수
	FTimerHandle PostLoginTimer;
	float LastPostLoginTime;
	void CheckPostLoginTimeOut();
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
    
    // 협동 미션용 함수 및 변수
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ShowClearUI();

    UPROPERTY()
    TArray<APlayerController*> Coop_LoggedInPlayers;

    bool bMissionIsComplete = false;
    bool bIsGameOver = false;
    bool bRolesAssigned = false;

	FTimerHandle RoleAssignmentTimer;
	void AttemptToAssignCoopRoles();
	
	FRandomStream RandomStream;
};
