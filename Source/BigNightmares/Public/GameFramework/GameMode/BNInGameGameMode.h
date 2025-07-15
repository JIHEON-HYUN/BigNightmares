// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BNInGameGameMode.generated.h"

class UBNMonoCharacterDataAsset;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNInGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;
	
private:
	// 서버에서만 처리해야할 데이터는 gamemode에 선언되어야 함. 해당 변수는 캐릭터의 기본 베이스 데이터에셋임.
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
};
