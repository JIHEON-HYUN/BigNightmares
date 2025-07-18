// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNLobbyPlayerList.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNLobbyPlayerList : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SteamID;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* CheckReady;
	
public:
	void Setup(FString NewSteamID, bool ReadyState);
	void ChangeReadyState();
	const FString GetSteamID();
};
