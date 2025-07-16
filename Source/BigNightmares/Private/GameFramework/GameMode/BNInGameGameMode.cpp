// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNInGameGameMode.h"

void ABNInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	
}

UBNMonoCharacterDataAsset* ABNInGameGameMode::GetBNMonoCharacterDataAsset() const
{
	return MonoCharacterDefaultDataAsset;
}

