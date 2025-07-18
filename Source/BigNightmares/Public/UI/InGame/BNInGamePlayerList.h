// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNInGamePlayerList.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNInGamePlayerList : public UUserWidget
{
	GENERATED_BODY()

protected:
	bool bStatusAlive = true;
	
	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SteamID;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* StatusAlive;
	
public:
	void Setup(FString NewSteamID, bool NewStatusAlive);
	void ChangeStatusAlive();
	const FString GetSteamID();
};
