// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNCoopMissionGameMode.generated.h"

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

protected:
	// 매 프레임마다 호출될 Tick 함수를 오버라이드합니다.
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	UPROPERTY()
	TArray<APlayerController*> LoggedInPlayers;

	// [해결책] 더 이상 필요 없는 boolean 플래그를 제거했습니다.
};
