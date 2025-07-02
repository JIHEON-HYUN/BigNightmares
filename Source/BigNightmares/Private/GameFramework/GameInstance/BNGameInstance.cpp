// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameInstance/BNGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

#include "UI/MainMenu/BNMainMenuWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");

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
			// CreateSession이 완료되면 델리게이트 호출
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBNGameInstance::OnCreateSessionComplete);
			// DestroySession이 완료되면 델리게이트 호출
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBNGameInstance::OnDestroySessionComplete);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionInterface->FindSessions(0, SessionSearch);
		}
	}
}

void UBNGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession == nullptr)
		{
			// 이미 존재하는 세션이 없으면 CreateSession
			CreateSession();
		}
		else
		{
			// DestroySession
			SessionInterface->DestroySession(SESSION_NAME);
		}
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

	// Address의 서버로 접속해서, 그 서버가 연 맵을 따라감
		// TRAVEL_Absolute - 전체 경로를 지정하여 해당 맵으로 이동 (가장 일반적임)
		// TRAVEL_Partial - 플레이어 상태를 유지한 채로 레벨만 전환 (Seamless Travel)
		// TRAVEL_Relative - 상대 경로 기반 이동 (거의 사용 안됨)
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

void UBNGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		// CreateSession
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UBNGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}
	
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->CloseMenu();
	}
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// 서버가 지정한 맵으로 현재 맵을 전환, 클라이언트도 자동으로 따라옴 (서버 전용)
	// ?listen을 붙이지 않으면 멀티플레이 세션이 아니므로, 클라이언트가 서버에 연결이 불가능
	World->ServerTravel("/Game/Levels/L_Lobby?listen");
}

void UBNGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CreateSession();
	}
}