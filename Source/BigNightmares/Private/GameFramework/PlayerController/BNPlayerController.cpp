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
#include "Interaction/Mission/VerticalTimingGaugeComponent.h"
#include "Net/UnrealNetwork.h"
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
void ABNPlayerController::Client_StartGaugeUI_Implementation(UVerticalTimingGaugeComponent* InGaugeComponent)
{
	if (!IsValid(this) || !IsValid(VerticalGaugeWidgetClass) || !IsValid(InGaugeComponent)) 
	{
		UE_LOG(LogTemp, Error, TEXT("Client_StartGaugeUI: PlayerController, Widget Class, or GaugeComponent invalid. Cannot create widget."));
		return;
	}

	//활성화된 위젯이 있다면 제거 (중복생성방지)
	if (IsValid(VerticalGaugeWidgetInstance))
	{
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;
	}

	//위젯 생성 시 OwningObject를 this(현재 로컬 플레이어 컨트롤러)로 설정해 생성
	VerticalGaugeWidgetInstance = CreateWidget<UBNInGameWidget>(this, VerticalGaugeWidgetClass);

	//위젯 생성 검사
	if (!IsValid(VerticalGaugeWidgetInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("Not Create VerticalGaugeWidgetInstance"));
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
		UE_LOG(LogTemp, Error, TEXT("Client_StartGaugeUI (PlayerController): Some internal widgets not found in created instance! Check widget names in BP."))

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

//Client_EndGaugeUI RPC 구현 
void ABNPlayerController::Client_EndGaugeUI_Implementation(EVerticalGaugeResult Result)
{
	if (IsValid(VerticalGaugeWidgetInstance))
	{
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;

		Border_GaugeBackground = nullptr;
		Image_Green = nullptr;
		Image_Pointer = nullptr;

		//브로드 캐스트를 2개 만들어서 일반적인 창 닫기와 퀘스트 성공시 창 닫는걸 분리할 예정
		if (ActiveGaugeComponent.IsValid())
		{
			ActiveGaugeComponent->OnGaugeFinished.Broadcast(Result);
		}
		ActiveGaugeComponent = nullptr; //참조 해제
	}
}

void ABNPlayerController::UpdateGreenZoneUI(float GreenZoneStart, float GreenZoneLength)
{
	if (IsValid(Image_Green) && IsValid(Border_GaugeBackground) && CachedGaugeHeight > 0.f)
	{
		//UCanvasPanelSlot은 Canvas Panel안에 있는 UI위젯의 위치,크기,정렬상태를 지정하는 정보
		UCanvasPanelSlot* GreenZoneSlot = Cast<UCanvasPanelSlot>(Image_Green->Slot);
		UCanvasPanelSlot* GaugeBackgroundSlot = Cast<UCanvasPanelSlot>(Border_GaugeBackground->Slot);

		if (IsValid(GreenZoneSlot) && IsValid(GaugeBackgroundSlot))
		{
			//Background의 Size가 0인지 확인하고 0이면 캐시값 사용
			//UWidget::GetCachedGeometry() : 현재 위젯이 화면에 렌더링될 때 계산된 위치/크기/변형 등의 정보를 담은 객체를 반환
			//반환 타입 : FGeometry
			//주의점 : 랜더링이 한 번 이상 되고 나서만 이 값이 정상적으로 채워짐.
			//FGeometry::GetLocalSize() : 해당 위젯의 로컬 좌표계 기준 너비, 높이 값을 반환
			//반환 타입 : FVector2D
			FVector2D BackgroundSize = Border_GaugeBackground->GetCachedGeometry().GetLocalSize();
			if (BackgroundSize.IsZero()) BackgroundSize = FVector2D(50.f, CachedGaugeHeight);

			float GaugeWidth = BackgroundSize.X;
			float GaugeHeight = BackgroundSize.Y;

			//GetPosition() : 캔버스 내에서 해당 위젯이 어디에 위치해 있는지 좌표값을 가져오는 것
			FVector2D GaugeBackgroundPos = GaugeBackgroundSlot->GetPosition();

			//GreenZone의 길이
			float ZonePixelHeight = GaugeHeight * GreenZoneLength;
			//GreenZone의 시작 위치 (1.0 - ((0 ~ 1-GreenZoneLength) + 0.25)) * 800;
			float ZonePixelY = (1.0 -(GreenZoneStart + GreenZoneLength)) * GaugeHeight;

			//GreenZone의 위치 및 크기를 세팅
			GreenZoneSlot->SetPosition(FVector2D(GaugeBackgroundPos.X, GaugeBackgroundPos.Y + ZonePixelY));
			GreenZoneSlot->SetSize(FVector2D(GaugeWidth, ZonePixelHeight));
		}
	}
}

void ABNPlayerController::UpdatePointerUI(float CurrentGaugeValue)
{
	if (IsValid(Image_Pointer) && IsValid(Border_GaugeBackground) && CachedGaugeHeight > 0.f && CachedPointerHeight > 0.f)
	{
		UCanvasPanelSlot* PointZoneSlot = Cast<UCanvasPanelSlot>(Image_Pointer->Slot);
		UCanvasPanelSlot* GaugeBackGroundSlot = Cast<UCanvasPanelSlot>(Border_GaugeBackground->Slot);

		if (IsValid(PointZoneSlot) && IsValid(GaugeBackGroundSlot))
		{
			FVector2D BackGroundSize = Border_GaugeBackground->GetCachedGeometry().GetLocalSize();
			if (BackGroundSize.IsZero()) BackGroundSize = FVector2D(50.f, CachedGaugeHeight);

			float GaugeHeight = BackGroundSize.Y;
			float PointHeight = CachedPointerHeight;

			FVector2D GaugeBackgroundPos = GaugeBackGroundSlot->GetPosition();

			float PointerYInGauge =  (1.0 -CurrentGaugeValue) * (GaugeHeight - PointHeight);
			float AbsolutePointerY = GaugeBackgroundPos.Y + PointerYInGauge;

			float SomeOffSet = 20.f; // 화살표의 처음 위치가 보더의 x좌표보다 20만큼 옆
			float PointerX = GaugeBackgroundPos.X + PointerYInGauge;

			AbsolutePointerY = FMath::Clamp(AbsolutePointerY, GaugeBackgroundPos.Y, GaugeBackgroundPos.Y + GaugeHeight - PointHeight);

			PointZoneSlot->SetPosition(FVector2D(PointerX, AbsolutePointerY));
		}
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
