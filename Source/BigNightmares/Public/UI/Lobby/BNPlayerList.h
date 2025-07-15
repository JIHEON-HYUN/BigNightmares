// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNPlayerList.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNPlayerList : public UUserWidget
{
	GENERATED_BODY()

public:
	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SteamID;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* CheckReady;

	void Setup(class UBNLobbyWidget* NewParent);
	void ChangeReadyState();

private:
	// 부모 로비 메뉴 위젯
	UPROPERTY()
	class UBNLobbyWidget* Parent;
};
