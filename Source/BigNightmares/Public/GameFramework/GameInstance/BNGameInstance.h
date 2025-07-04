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
	virtual void Host(FString SessionName) override;
	
	UFUNCTION()
	virtual void Join(uint32 Index) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Quit() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void LoadMainMenu() override;

	UFUNCTION(BlueprintCallable)
	virtual void RefreshSessionList() override;

	void StartSession();

private:
	TSubclassOf<class UUserWidget> MainMenuClass;
	class UBNMainMenuWidget* MainMenuWidget;

	FString DesiredSessionName;

	// typedef TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> IOnlineSessionPtr (shared포인터)
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void CreateSession();

	// Callback Func
		// Dynamic이 아닌 일반 델리게이트이므로, UFUNCTION() 등록 안해도 됨
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnNetworkFailure(UWorld * World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
