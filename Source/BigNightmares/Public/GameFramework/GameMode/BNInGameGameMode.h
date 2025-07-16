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
	virtual void BeginPlay() override;
	
	UBNMonoCharacterDataAsset* GetBNMonoCharacterDataAsset() const;
	
private:
	// 캐릭터 기본 베이스 데이터에셋
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Class Defaults")
	TObjectPtr<UBNMonoCharacterDataAsset> MonoCharacterDefaultDataAsset;
};
