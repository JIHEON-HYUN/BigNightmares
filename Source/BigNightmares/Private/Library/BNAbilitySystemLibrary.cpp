// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/BNAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "GameFramework/GameMode/BNInGameGameMode.h"

UBNMonoCharacterDataAsset* UBNAbilitySystemLibrary::GetMonoCharacterDataAsset(const UObject* WorldContextObject)
{
	if (const ABNInGameGameMode* BNInGameGameMode = Cast<ABNInGameGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return BNInGameGameMode->GetBNMonoCharacterDataAsset();
	}

	return nullptr;
}
