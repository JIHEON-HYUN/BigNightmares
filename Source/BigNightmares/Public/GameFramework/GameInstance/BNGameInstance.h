// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BNGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UBNGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Init() override;
};
