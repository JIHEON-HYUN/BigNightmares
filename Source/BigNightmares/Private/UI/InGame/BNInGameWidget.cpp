// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNInGameWidget.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "GameFramework/PlayerState.h"

#include "GameFramework/GameMode/BNInGameGameMode.h"
#include "UI/InGame/BNInGameInterface.h"
#include "UI/InGame/BNInGamePlayerList.h"

UBNInGameWidget::UBNInGameWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// WBP_PlayerList를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> InGamePlayerListBPClass(TEXT("/Game/UI/InGame/WBP_InGamePlayerList"));
	if (InGamePlayerListBPClass.Succeeded())
	{
		InGamePlayerListClass = InGamePlayerListBPClass.Class;
	}
}

bool UBNInGameWidget::Initialize()
{
	bool SuperSuccess = Super::Initialize();
	if (!SuperSuccess) return false;

	// 버튼 클릭 델리게이트 바인딩
	if (Btn_Exit == nullptr) return false;
	Btn_Exit->OnClicked.AddDynamic(this, &UBNInGameWidget::OnClickedExit);
	
	return true;
}

void UBNInGameWidget::SetInGameInterface(IBNInGameInterface* NewInGameInterface)
{
	InGameInterface = NewInGameInterface;
}

void UBNInGameWidget::SetPlayerList(const TArray<FInGamePlayerData>& PlayerDataList)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	// 위젯 생성 전, 이전 결과 초기화
	PlayerListBox->ClearChildren();
	
	for (const FInGamePlayerData& PlayerData : PlayerDataList)
	{
		auto Player = CreateWidget<UBNInGamePlayerList>(World, InGamePlayerListClass);
		if (Player == nullptr) return;
		
		Player->Setup(PlayerData.PlayerName, PlayerData.StatusAlive);

		PlayerListBox->AddChild(Player);
	}
}

void UBNInGameWidget::OpenMenu()
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

void UBNInGameWidget::CloseMenu()
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

void UBNInGameWidget::OnClickedExit()
{
	InGameInterface->ReturnToMainMenu();
}
