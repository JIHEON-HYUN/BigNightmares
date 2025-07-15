// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BNLobbyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBNLobbyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BIGNIGHTMARES_API IBNLobbyInterface
{
	GENERATED_BODY()

public:
	virtual void LoadLobbyMenu() = 0;
	virtual void OpenLobbyMenu() = 0;
	virtual void CloseLobbyMenu() = 0;
	virtual void ReturnToMainMenu() = 0;
	virtual void ChangePlayerReadyState() = 0;
};
