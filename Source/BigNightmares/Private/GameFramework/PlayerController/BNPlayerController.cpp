// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerController/BNPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Net/VoiceConfig.h"
#include "TimerManager.h"
#include "VoiceModule.h"
#include "Interfaces/VoiceCapture.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Interaction/Mission/MissionTimingGauge.h"
#include "Interaction/Mission/VerticalTimingGaugeComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/InGame/BNInventoryWidgetController.h"
#include "UI/InGame/BNSystemWidget.h"
#include "UI/InGame/BNInGameWidget.h"
#include "UI/InGame/BNMission1Widget.h"
#include "UI/Lobby/BNLobbyWidget.h"

ABNPlayerController::ABNPlayerController()
{
	// WBP_Lobby를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> LobbyBPClass(TEXT("/Game/UI/Lobby/WBP_Lobby"));
	if (LobbyBPClass.Succeeded())
	{
		LobbyClass = LobbyBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;

	//초기값설정
	CachedGaugeHeight = 0.f;
	CachedPointerHeight = 0.f;
}

void ABNPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS == nullptr) return;
	
	GS->OnLobbyPlayerUpdated.AddDynamic(this, &ABNPlayerController::OnLobbyPlayerUpdated_Handler);

	// Steam 보이스 시스템에서 이 클라이언트의 Talker 생성
	if (IsLocalController())
	{
		auto PS = GetPlayerState<ABNPlayerState>();
		if (PS == nullptr) return;
		
		UVOIPTalker* Talker = UVOIPTalker::CreateTalkerForPlayer(PS);
		if (Talker)
		{
			Talker->RegisterWithPlayerState(PS);
			UE_LOG(LogTemp, Error, TEXT("hihihihihihihihihiihihihihihi234234234234234"));

			this->StartTalking();
		}
	}

	// 로컬 플레이어 UI 위젯 클래스 미리 로드 예정
}

void ABNPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABNPlayerController, bIsGaugeActive);
}

#pragma region Lobby
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

#pragma endregion Lobby

#pragma region Inventory
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

bool ABNPlayerController::Server_ReportGaugeInput_Validate(FGuid InGaugeID, float SmoothedGaugeValue)
{
	if (!InGaugeID.IsValid())
	{
		return false;
	}

	//KINDA_SMALL_NUMBER : float의 값을 0과 비교할 때 미세한 오차 범위 내에 있는지 판단필요, 언리얼 엔진에 무시 가능한 오차를 측정할 때 사용하도록 만든 매크로
	if (SmoothedGaugeValue < -KINDA_SMALL_NUMBER || SmoothedGaugeValue > KINDA_SMALL_NUMBER + 1.0f)
	{
		return false;
	}
	
	return true;
}

void ABNPlayerController::Server_ReportGaugeInput_Implementation(FGuid InGaugeID, float SmoothedGaugeValue)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (ActiveGaugeComponent.IsValid())
	{
		if (ActiveGaugeComponent->GaugeID == InGaugeID)
		{
			AMissionTimingGauge* OwningMissionGauge = Cast<AMissionTimingGauge>(ActiveGaugeComponent->GetOwner());

			if (IsValid(OwningMissionGauge))
			{
				OwningMissionGauge->Server_PerformGaugeCheck(InGaugeID, SmoothedGaugeValue);
			}			
		}
	}
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

UAbilitySystemComponent* ABNPlayerController::GetAbilitySystemComponent() const
{
	return Cast<UBNBaseAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
}

#pragma endregion Inventory

#pragma region Missions1

// Client_StartGaugeUI RPC 구현
void ABNPlayerController::Client_ShowMission1GaugeUI_Implementation(UVerticalTimingGaugeComponent* InGaugeComponent, int32 MaxLife, int32 RequiredSuccess)
{
	if (!IsValid(this) || !IsValid(Mission1WidgetClass) || !IsValid(InGaugeComponent)) 
	{
		UE_LOG(LogTemp, Error, TEXT("Client_StartGaugeUI: PlayerController, Widget Class, or GaugeComponent invalid. Cannot create widget."));
		return;
	}
	
	//활성화된 위젯이 있다면 제거 (중복생성방지)
	if (IsValid(Mission1WidgetInstance))
	{
		Mission1WidgetInstance->RemoveFromParent();
		Mission1WidgetInstance = nullptr;
	}
	
	//위젯 생성 시 OwningObject를 this(현재 로컬 플레이어 컨트롤러)로 설정해 생성
	Mission1WidgetInstance = CreateWidget<UBNMission1Widget>(this, Mission1WidgetClass);
	
	//위젯 생성 검사
	if (!IsValid(Mission1WidgetInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("Not Create VerticalGaugeWidgetInstance"));
		return;		
	}
	
	Mission1WidgetInstance->AddToViewport();
	Mission1WidgetInstance->SetGaugeComponent(InGaugeComponent);
	
	//현재 활성화된 게이지 컴포넌트 참조 저장
	ActiveGaugeComponent = InGaugeComponent;

	Mission1WidgetInstance->SetMissionGoals(MaxLife, RequiredSuccess);

	if (IsValid(Mission1WidgetInstance))
	{
		// 사용 사례에 따라 FInputModeUIOnly 또는 FInputModeGameAndUI 중 선택합니다.
		// FInputModeGameAndUI: 게임 입력과 UI 입력을 모두 받습니다. (권장)
		// FInputModeUIOnly: UI 입력만 받으며 게임 입력은 막힙니다.
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(Mission1WidgetInstance->TakeWidget());
		SetInputMode(InputMode);

		Mission1WidgetInstance->SetKeyboardFocus(); // 위젯에 명시적으로 키보드 포커스 부여
	}
	
	UE_LOG(LogTemp, Log, TEXT("Client (PlayerController): Gauge UI fully initialized and started for Player %s."), *GetName());
}

//Client_EndGaugeUI RPC 구현 
void ABNPlayerController::Client_EndGaugeUI_Implementation(EVerticalGaugeResult Result)
{
	if (IsValid(Mission1WidgetInstance))
	{
		Mission1WidgetInstance->RemoveFromParent();
		Mission1WidgetInstance = nullptr;
		
		ActiveGaugeComponent = nullptr; //참조 해제
	}
}

bool ABNPlayerController::Server_NotifyGaugeFinished_Validate(FGuid InGaugeID, EVerticalGaugeResult Result)
{
	return true;
}

void ABNPlayerController::Server_NotifyGaugeFinished_Implementation(FGuid InGaugeID, EVerticalGaugeResult Result)
{
	if (!HasAuthority()) return;

	ABNGameState* GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS)
	{
		GS->Server_EndSpecificGaugeChallenge(InGaugeID, this);
	}

	Client_EndGaugeUI(Result);

	UE_LOG(LogTemp, Display, TEXT("Server EndGauge"));
}
#pragma endregion Missions1
