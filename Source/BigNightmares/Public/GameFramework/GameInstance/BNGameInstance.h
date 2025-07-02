// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UI/MainMenu/BNMainMenuInterface.h"
#include "BNGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNGameInstance : public UGameInstance, public IBNMainMenuInterface
{
	GENERATED_BODY()

public:
	UBNGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Init() override;

	// MainMenu 기능
	UFUNCTION()
	virtual void Host() override;
	UFUNCTION()
	virtual void Join(const FString& Address) override;
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

private:
	TSubclassOf<class UUserWidget> MainMenuClass;
	class UBNMainMenuWidget* MainMenuWidget;
	
	IOnlineSessionPtr SessionInterface;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
};
