// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameInstance/BNGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Player/BNMonoCharacter.h"

#include "UI/MainMenu/BNMainMenuWidget.h"

UBNGameInstance::UBNGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/UI/MainMenu/WBP_MainMenu"));
	if (MainMenuBPClass.Succeeded())
	{
		MainMenuClass = MainMenuBPClass.Class;
	}
}

void UBNGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBNGameInstance::OnCreateSessionComplete);
		}
	}
}

void UBNGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		// CreateSession이 완료되면 델리게이트 호출
		SessionInterface->CreateSession(0, TEXT("My Session Game"), SessionSettings);
	}
}

void UBNGameInstance::Join(const FString& Address)
{
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->CloseMenu();
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) return;

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UBNGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;

	MainMenuWidget = CreateWidget<UBNMainMenuWidget>(this, MainMenuClass);
	if (MainMenuWidget == nullptr) return;

	MainMenuWidget->Setup();
	MainMenuWidget->SetIBNMainMenuInterface(this);
}

void UBNGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->CloseMenu();
	}
	
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		return;
	}
	
	World->ServerTravel("/Game/Levels/L_Lobby?listen");
}
