// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameInstance/BNGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/KismetSystemLibrary.h"

#include "UI/MainMenu/BNMainMenuWidget.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SESSION_NAME_SETTINGS_KEY = TEXT("SessionName");

UBNGameInstance::UBNGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// WBP_MainMenu를 찾아서 저장
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
			// FindSessions가 완료되면 델리게이트 호출
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBNGameInstance::OnFindSessionsComplete);
			// JoinSession이 완료되면 델리게이트 호출
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBNGameInstance::OnJoinSessionComplete);
		}
	}

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UBNGameInstance::OnNetworkFailure);
	}
}

void UBNGameInstance::Host(FString SessionName)
{
	DesiredSessionName = SessionName;
	
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

void UBNGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;
	
	if (MainMenuWidget != nullptr)
	{
		MainMenuWidget->CloseMenu();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UBNGameInstance::Quit()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	
	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, false);
}

void UBNGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;

	MainMenuWidget = CreateWidget<UBNMainMenuWidget>(this, MainMenuClass);
	if (MainMenuWidget == nullptr) return;

	MainMenuWidget->OpenMenu();
	MainMenuWidget->SetMainMenuInterface(this);
}

void UBNGameInstance::RefreshSessionList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	if (SessionSearch.IsValid())
	{
		// bIsLanQuery : Lan 세션만 검색(true), 온라인 세션만 검색(false)
		// SessionSearch->bIsLanQuery = true; : Lan 세션만 검색

		// MaxSearchResults의 기본 값이 1밖에 안되기 때문에 100으로 설정
		SessionSearch->MaxSearchResults = 100;
		// QuerySettings.Set(필터 기준, 비교 값, 비교 방식);
			// 필터 기준 : 온라인 상태 표시가 된 세션 (Presence가 있는 세션)
			// 비교 값 : true
			// 비교 방식 : Equals
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		
		// SessionSearch가 유효하면(null이 아니면) SharedPtr를 SharedRef로 변환하여 FindSessions (SharedRef는 null이 아님이 보장되야 함)
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UBNGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}
		
		SessionSettings.NumPublicConnections = 12;
		// 온라인 세션 검색에 노출할지 결정, false면 세션 검색에서 제외됨
		SessionSettings.bShouldAdvertise = true;
		// Presence 세션으로 등록되서, Presence 기반 search에 노출됨
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.Set(SESSION_NAME_SETTINGS_KEY, DesiredSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		// CreateSession
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UBNGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
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

void UBNGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	LoadMainMenu();
}

void UBNGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid() && MainMenuWidget != nullptr)
	{
		TArray<FSessionData> SessionDatas;
		for (const auto& SearchResult : SessionSearch->SearchResults )
		{
			FSessionData SessionData;
			SessionData.SessionName = SearchResult.GetSessionIdStr();
			SessionData.MaxPlayer = SearchResult.Session.SessionSettings.NumPublicConnections;
			SessionData.CurrentPlayer = SessionData.MaxPlayer - SearchResult.Session.NumOpenPublicConnections;
			SessionData.HostUserName = SearchResult.Session.OwningUserName;

			FString SessionName;
			if (SearchResult.Session.SessionSettings.Get(SESSION_NAME_SETTINGS_KEY, SessionName))
			{
				SessionData.SessionName = SessionName;
			}
			else
			{
				SessionData.SessionName = "Could not find name";
			}
			
			SessionDatas.Add(SessionData);
		}

		MainMenuWidget->SetSessionList(SessionDatas);
	}
}

void UBNGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;
	
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController == nullptr) return;

	FString Address;
	// Address를 참조(&)로 넘겨 SessionName에 대한 실제 네트워크 주소를 아웃 파라미터로 전달 받음
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not connect session"));
		return;
	}
	
	// Address의 서버로 접속해서, 그 서버가 연 맵을 따라감
		// TRAVEL_Absolute - 전체 경로를 지정하여 해당 맵으로 이동 (가장 일반적임)
		// TRAVEL_Partial - 플레이어 상태를 유지한 채로 레벨만 전환 (Seamless Travel)
		// TRAVEL_Relative - 상대 경로 기반 이동 (거의 사용 안됨)
	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

