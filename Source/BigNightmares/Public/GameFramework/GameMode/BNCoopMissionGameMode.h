// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNCoopMissionGameMode.generated.h"

class UUserWidget; // [추가] UUserWidget 클래스 전방 선언

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNCoopMissionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 생성자를 추가하여 Tick을 활성화합니다.
	ABNCoopMissionGameMode();

	// [추가] ChestActor가 호출할 미션 완료 함수입니다.
	void MissionComplete();

	// [추가] 씬맨이 호출할, 모든 플레이어를 사망 처리하는 함수입니다.
	void AllPlayersKilledByThinman(AActor* Killer);

	// [추가] 게임 오버 상태를 외부에서 조회할 수 있는 함수입니다.
	bool IsGameOver() const { return bIsGameOver; }

protected:
	// 매 프레임마다 호출될 Tick 함수를 오버라이드합니다.
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// [추가] 에디터에서 지정할 클리어 UI 위젯 클래스입니다.
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MissionClearWidgetClass;

private:
	// [추가] 모든 클라이언트에게 클리어 UI를 보여주라고 명령하는 함수입니다.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowClearUI();
	
	UPROPERTY()
	TArray<APlayerController*> LoggedInPlayers;

	// [추가] 미션이 이미 클리어됐는지 확인하는 변수입니다.
	bool bMissionIsComplete = false;
	
	// [추가] 게임이 끝났는지(모든 플레이어가 사망했는지) 확인하는 변수입니다.
	bool bIsGameOver = false;
};
