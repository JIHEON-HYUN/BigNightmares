// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerController/BNPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/VoiceConfig.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "UI/InGame/BNInventoryWidgetController.h"
#include "UI/InGame/BNSystemWidget.h"
#include "UI/InGame/BNInGameWidget.h"
#include "UI/Lobby/BNLobbyWidget.h"

ABNPlayerController::ABNPlayerController()
{
	// WBP_Lobby를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> LobbyBPClass(TEXT("/Game/UI/Lobby/WBP_Lobby"));
	if (LobbyBPClass.Succeeded())
	{
		LobbyClass = LobbyBPClass.Class;
	}

	// WBP_InGame를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> InGameBPClass(TEXT("/Game/UI/InGame/Menu/WBP_InGame"));
	if (InGameBPClass.Succeeded())
	{
		InGameClass = InGameBPClass.Class;
	}
}

void ABNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	GS->OnLobbyPlayerUpdated.AddDynamic(this, &ABNPlayerController::OnLobbyPlayerUpdated_Handler);
	GS->OnInGamePlayerUpdated.AddDynamic(this, &ABNPlayerController::OnInGamePlayerUpdated_Handler);

	// Steam 보이스 시스템에서 이 클라이언트의 Talker 생성
	if (IsLocalController())
	{
		auto PS = GetPlayerState<ABNPlayerState>();
		if (PS == nullptr) return;
		
		UVOIPTalker* Talker = UVOIPTalker::CreateTalkerForPlayer(PS);
		if (Talker)
		{
			Talker->RegisterWithPlayerState(PS);

			this->StartTalking();
		}
	}
}

void ABNPlayerController::LoadLobbyMenu()
{
	if (LobbyClass == nullptr) return;
	
	LobbyWidget = CreateWidget<UBNLobbyWidget>(this, LobbyClass);
	if (LobbyWidget == nullptr) return;

	LobbyWidget->SetLobbyInterface(this);
}

void ABNPlayerController::OpenLobbyMenu()
{
	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	LobbyWidget->SetPlayerList(GS->GetLobbyPlayers());
	LobbyWidget->OpenMenu();
}

void ABNPlayerController::CloseLobbyMenu()
{
	LobbyWidget->CloseMenu();
}

void ABNPlayerController::ReturnToMainMenu()
{
	ClientTravel("/Game/Levels/L_MainMenu", TRAVEL_Absolute);
}

void ABNPlayerController::ChangePlayerReadyState_Implementation()
{
	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	auto PS = GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;

	GS->SetPlayerReadyState(PS->GetPlayerName());
}

void ABNPlayerController::OnLobbyPlayerUpdated_Handler(const TArray<FLobbyPlayerData>& UpdatedList)
{
	if (LobbyWidget)
	{
		LobbyWidget->SetPlayerList(UpdatedList);
	}
}

void ABNPlayerController::LoadInGameMenu()
{
	if (InGameClass == nullptr) return;
	
	InGameWidget = CreateWidget<UBNInGameWidget>(this, InGameClass);
	if (InGameWidget == nullptr) return;

	InGameWidget->SetInGameInterface(this);
}

void ABNPlayerController::OpenInGameMenu()
{
	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	InGameWidget->SetPlayerList(GS->GetInGamePlayers());
	InGameWidget->OpenMenu();
}

void ABNPlayerController::CloseInGameMenu()
{
	InGameWidget->CloseMenu();
}

void ABNPlayerController::ChangePlayerStatusAlive_Implementation()
{
	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	auto PS = GetPlayerState<ABNPlayerState>();
	if (PS == nullptr) return;

	GS->SetPlayerStatusAlive(PS->GetPlayerName());
}

void ABNPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABNPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UE_LOG(LogTemp, Log, TEXT("Client %d: OnRep_PlayerState called."), GetLocalPlayer()->GetControllerId());

	TryInitializeRpcReadiness();
}

void ABNPlayerController::TryInitializeRpcReadiness()
{
	if (bCanCallServerRPC)
	{
		return;
	}

	ABNPlayerState* BNPlayerState = GetPlayerState<ABNPlayerState>();
	if (IsValid(BNPlayerState))
	{
		ABNPlayerController* PlayerStateOwner = Cast<ABNPlayerController>(BNPlayerState->GetOwner());
		if (IsValid(PlayerStateOwner) && PlayerStateOwner == this)
		{
			// 이제 PlayerState의 Owner가 현재 컨트롤러로 제대로 설정.
			bCanCallServerRPC = true;

			GetWorldTimerManager().ClearTimer(InitHandle);
		}
		else
		{
			// Owner가 아직 유효하지 않거나 자신과 매칭되지 않음 (Run Under One Process에서 발생 가능)
			GetWorldTimerManager().SetTimer(InitHandle, this, &ABNPlayerController::TryInitializeRpcReadiness, 0.1f, false);
		}
	}
}

void ABNPlayerController::OnInGamePlayerUpdated_Handler(const TArray<FInGamePlayerData>& UpdatedList)
{
	if (InGameWidget)
	{
		InGameWidget->SetPlayerList(UpdatedList);
	}
}

UAbilitySystemComponent* ABNPlayerController::GetAbilitySystemComponent() const
{
	return Cast<UBNBaseAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
}

UBNInventoryWidgetController* ABNPlayerController::GetBNInventoryWidgetController()
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UBNInventoryWidgetController>(this, BNInventoryWidgetControllerClass);
		InventoryWidgetController->SetOwningActor(GetPlayerState<ABNPlayerState>());
		InventoryWidgetController->BindCallbacksToDependencies();
	}

	return InventoryWidgetController;
}

void ABNPlayerController::CreateInventoryWidget()
{
	if (UUserWidget* Widget = CreateWidget<UBNSystemWidget>(this, InventoryWidgetClass))
	{
		InventoryWidget = Cast<UBNSystemWidget>(Widget);
		InventoryWidget->SetWidgetController(GetBNInventoryWidgetController());
		InventoryWidgetController->BroadcastInitialValues();
		InventoryWidget->AddToViewport();
	}
}
