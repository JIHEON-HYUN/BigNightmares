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
#include "Components/Image.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Interaction/Mission/VerticalTimingGaugeComponent.h"
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

void ABNPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalController() && IsValid(VerticalGaugeWidgetInstance) && ActiveGaugeComponent.IsValid())
	{
		UpdatePointerUI(ActiveGaugeComponent->CurrentGaugeValue);
	}
}

void ABNPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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
void ABNPlayerController::Client_StartGaugeUI_Implementation(UVerticalTimingGaugeComponent* InGaugeComponent)
{
	if (!IsValid(this) || !IsValid(VerticalGaugeWidgetClass) || !IsValid(InGaugeComponent)) 
	{
		UE_LOG(LogTemp, Error, TEXT("Client_StartGaugeUI: PlayerController, Widget Class, or GaugeComponent invalid. Cannot create widget."));
		return;
	}

	//활성화된 위젝이 있다면 제거 (중복생성방지)
	if (IsValid(VerticalGaugeWidgetInstance))
	{
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;
	}

	VerticalGaugeWidgetInstance = CreateWidget<UBNInGameWidget>(this, VerticalGaugeWidgetClass);

	if (!IsValid(VerticalGaugeWidgetInstance))
	{
		return;		
	}
	
	VerticalGaugeWidgetInstance->AddToViewport();

	//현재 활성화된 게이지 컴포넌트 참조 저장
	ActiveGaugeComponent = InGaugeComponent;

	//위젯 내부의 요소들을 가져옴
	Border_GaugeBackground = Cast<UBorder>(VerticalGaugeWidgetInstance->GetWidgetFromName(TEXT("Border_GaugeBackground")));
	Image_Green = Cast<UImage>(VerticalGaugeWidgetInstance->GetWidgetFromName(TEXT("Image_Green")));
	Image_Pointer = Cast<UImage>(VerticalGaugeWidgetInstance->GetWidgetFromName(TEXT("Image_Pointer")));

	if (!IsValid(Border_GaugeBackground) || !IsValid(Image_Green) || !IsValid(Image_Pointer))
	{
		UE_LOG(LogTemp, Error, TEXT("Client_StartGaugeUI (PlayerController): Some internal widgets not found in created instance! Check widget names in BP."));

		// 위젯 내부요소가 없으면 위젯을 제거, 반환하는 것이 안전
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;
		ActiveGaugeComponent = nullptr;
		return;
	}

	//초기 UI업데이트
	UpdateGreenZoneUI(InGaugeComponent->GreenZoneStart, InGaugeComponent->GreenZoneLength);
	UpdatePointerUI(InGaugeComponent->CurrentGaugeValue);

	UE_LOG(LogTemp, Log, TEXT("Client (PlayerController): Gauge UI fully initialized and started for Player %s."), *GetName());
}

void ABNPlayerController::Client_EndGaugeUI_Implementation(EVerticalGaugeResult Result)
{
	
}

void ABNPlayerController::UpdateGreenZoneUI(float GreenZoneStart, float GreenZoneLength)
{
}

void ABNPlayerController::UpdatePointerUI(float CurrentGaugeValue)
{
}

bool ABNPlayerController::Server_NotifyGaugeFinished_Validate(FGuid InGaugeID, EVerticalGaugeResult Result)
{
	return true;
}

void ABNPlayerController::Server_NotifyGaugeFinished_Implementation(FGuid InGaugeID, EVerticalGaugeResult Result)
{
}

#pragma endregion Missions1
