// BNInGameGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNInGameGameMode.generated.h"

class UBNMonoCharacterDataAsset;

// 메인 게임에서 사용되는 플레이어 직업 enum
UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	None,
	Prey,
	Resident
};

// 메인 게임에서 사용되는 플레이어 데이터 구조체
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

	// 메인 게임의 플레이어 입장/퇴장 처리
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
    
	// 메인 게임의 플레이어 사망 처리
	void PlayerDead();
	
	// 모든 플레이어를 로비로 돌려보내는 함수
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PrivateMissionEnd();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AssignableMissionEnd();
	
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;

private:
	// 메인 게임의 역할 배정을 위한 타이머 및 변수
	FTimerHandle PostLoginTimer;
	float LastPostLoginTime;
	void CheckPostLoginTimeOut(); 
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
};
