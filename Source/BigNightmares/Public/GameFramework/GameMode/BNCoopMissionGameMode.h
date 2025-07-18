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

protected:
	// 새로운 플레이어가 게임에 로그인(접속)할 때마다 호출되는 함수입니다.
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	// [수정] 접속한 플레이어 컨트롤러들을 저장하기 위한 배열입니다.
	UPROPERTY()
	TArray<APlayerController*> LoggedInPlayers;
};
