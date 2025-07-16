// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerController/BNPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "UI/InGame/BNInventoryWidgetController.h"
#include "UI/InGame/BNSystemWidget.h"
#include "UI/Lobby/BNLobbyWidget.h"

ABNPlayerController::ABNPlayerController()
{
	// WBP_Lobby를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> LobbyBPClass(TEXT("/Game/UI/Lobby/WBP_Lobby"));
	if (LobbyBPClass.Succeeded())
	{
		LobbyClass = LobbyBPClass.Class;
	}
}

void ABNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	GS->OnLobbyPlayerUpdated.AddDynamic(this, &ABNPlayerController::OnLobbyPlayerUpdated_Handler);
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

	UE_LOG(LogTemp, Warning, TEXT("%s"), *PS->GetPlayerName());

	GS->SetPlayerReadyState(PS->GetPlayerName());
}

void ABNPlayerController::OnLobbyPlayerUpdated_Handler(const TArray<FLobbyPlayerData>& UpdatedList)
{
	if (LobbyWidget)
	{
		LobbyWidget->SetPlayerList(UpdatedList);
	}
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
			UE_LOG(LogTemp, Warning, TEXT("Client %d: PlayerState Owner is valid (%s). RPCs are now enabled."),
				  GetLocalPlayer()->GetControllerId(), *GetNameSafe(PlayerStateOwner));

			GetWorldTimerManager().ClearTimer(InitHandle);
		}
		else
		{
			// Owner가 아직 유효하지 않거나 자신과 매칭되지 않음 (Run Under One Process에서 발생 가능)
			UE_LOG(LogTemp, Warning, TEXT("Client %d: PlayerState Owner is NOT yet valid or not matching this controller (Owner: %s, This: %s). Retrying..."),
				   GetLocalPlayer()->GetControllerId(),
				   IsValid(PlayerStateOwner) ? *GetNameSafe(PlayerStateOwner) : TEXT("NULL"),
				   *GetNameSafe(this));
			GetWorldTimerManager().SetTimer(InitHandle, this, &ABNPlayerController::TryInitializeRpcReadiness, 0.1f, false);
		}
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
