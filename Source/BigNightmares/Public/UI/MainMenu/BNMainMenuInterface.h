// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BNMainMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBNMainMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BIGNIGHTMARES_API IBNMainMenuInterface
{
	GENERATED_BODY()

public:
	virtual void Host() = 0;
	virtual void Join(const FString& Address) = 0;
};
