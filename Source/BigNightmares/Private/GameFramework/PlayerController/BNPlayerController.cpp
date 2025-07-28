// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/PlayerController/BNPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/VoiceConfig.h"
#include "Blueprint/UserWidget.h"
#include "Net/VoiceConfig.h"
#include "TimerManager.h"
#include "VoiceModule.h"
#include "Interfaces/VoiceCapture.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
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
	// WBP_InGame를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> InGameBPClass(TEXT("/Game/UI/InGame/Menu/WBP_InGame"));
	if (InGameBPClass.Succeeded())
	{
		InGameClass = InGameBPClass.Class;
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

	// 로컬 플레이어 UI 위젯 클래스 미리 로드 예정


	//UI오픈상태에서 알트탭했을 때 위젯이 포커스를 잃어버리는 현상 수정을 위한 델리게이트
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &ABNPlayerController::OnApplicationStateChanged);
	}
}

void ABNPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().RemoveAll(this);
	}
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

UAbilitySystemComponent* ABNPlayerController::GetAbilitySystemComponent() const
{
	return Cast<UBNBaseAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
}

#pragma endregion Inventory

#pragma region Missions1

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


// Client_StartGaugeUI RPC 구현
void ABNPlayerController::Client_ShowMission1GaugeUI_Implementation(UVerticalTimingGaugeComponent* InGaugeComponent, int32 MaxLife, int32 RequiredSuccess)
{
	if (!IsValid(this) || !IsValid(Mission1WidgetClass) || !IsValid(InGaugeComponent)) 
	{
		UE_LOG(LogTemp, Error, TEXT("ABNPlayerController::Client_StartGaugeUI: PlayerController, Widget Class, or GaugeComponent invalid. Cannot create widget."));
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
		UE_LOG(LogTemp, Error, TEXT("ABNPlayerController::Client_StartGaugeUI: Not Create VerticalGaugeWidgetInstance"));
		return;		
	}
	
	Mission1WidgetInstance->AddToViewport();
	Mission1WidgetInstance->SetGaugeComponent(InGaugeComponent);
	
	//현재 활성화된 게이지 컴포넌트 참조 저장
	ActiveGaugeComponent = InGaugeComponent;

	Mission1WidgetInstance->SetMissionGoals(MaxLife, RequiredSuccess);

	if (IsValid(Mission1WidgetInstance))
	{
		// FInputModeUIOnly: UI 입력만 받으며 게임 입력은 막힙니다.
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(Mission1WidgetInstance->TakeWidget());
		SetShowMouseCursor(true);
		SetInputMode(InputMode);

		Mission1WidgetInstance->SetKeyboardFocus(); // 위젯에 명시적으로 키보드 포커스 부여
	}
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

#pragma endregion Missions1

//Alt + Tab으로 인해 인게임에서 focus를 잃어버렸을 때 다시 가지게 하는 함수
void ABNPlayerController::OnApplicationStateChanged(bool bIsActive)
{
	if (bIsActive)
	{
		if (IsValid(Mission1WidgetInstance) && Mission1WidgetInstance->IsInViewport())
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(Mission1WidgetInstance->TakeWidget());
			SetShowMouseCursor(true);
			SetInputMode(InputMode);

			Mission1WidgetInstance->SetKeyboardFocus();
		}
	}
}