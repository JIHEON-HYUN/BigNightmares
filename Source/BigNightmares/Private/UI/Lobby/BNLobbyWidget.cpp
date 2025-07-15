// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/BNLobbyWidget.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/Lobby/BNPlayerList.h"

#include "GameFramework/GameMode/BNLobbyGameMode.h"

UBNLobbyWidget::UBNLobbyWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// WBP_PlayerList를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> PlayerListBPClass(TEXT("/Game/UI/Lobby/WBP_PlayerList"));
	if (PlayerListBPClass.Succeeded())
	{
		PlayerListClass = PlayerListBPClass.Class;
	}
}

bool UBNLobbyWidget::Initialize()
{
	bool SuperSuccess = Super::Initialize();
	if (!SuperSuccess) return false;

	// 버튼 클릭 델리게이트 바인딩
	if (Btn_Exit == nullptr) return false;
	Btn_Exit->OnClicked.AddDynamic(this, &UBNLobbyWidget::OnClickedExit);

	if (Btn_Ready == nullptr) return false;
	Btn_Ready->OnClicked.AddDynamic(this, &UBNLobbyWidget::OnClickedReady);
	
	return true;
}

void UBNLobbyWidget::SetLobbyInterface(IBNLobbyInterface* NewLobbyInterface)
{
	LobbyInterface = NewLobbyInterface;
}

void UBNLobbyWidget::SetPlayerList(const TArray<FLobbyPlayerData>& PlayerDataList)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	// 위젯 생성 전, 이전 결과 초기화
	PlayerListBox->ClearChildren();
	
	for (const FLobbyPlayerData& PlayerData : PlayerDataList)
	{
		auto Player = CreateWidget<UBNPlayerList>(World, PlayerListClass);
		if (Player == nullptr) return;
		
		Player->SteamID->SetText(FText::FromString(PlayerData.PlayerName));

		Player->Setup(this);
		PlayerListBox->AddChild(Player);
	}
}

void UBNLobbyWidget::OpenMenu()
{
	// 로비 메뉴 화면에 띄우기
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr) return;

	// InputMode 설정 (UIOnly, Mouse 화면 밖에 벗어날 수 있게, 마우스 커서 보이게)
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UBNLobbyWidget::CloseMenu()
{
	// 로비 메뉴 화면에서 지우기
	this->RemoveFromParent();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr) return;

	// InputMode 설정 (GameOnly, 마우스 커서 안 보이게)
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}

void UBNLobbyWidget::OnClickedExit()
{
	LobbyInterface->ReturnToMainMenu();
}

void UBNLobbyWidget::OnClickedReady()
{
	for (int i = 0; i < PlayerListBox->GetChildrenCount(); i++)
	{
		Cast<UBNPlayerList>(PlayerListBox->GetChildAt(i))->ChangeReadyState();
	}
}
