// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BNMainMenuWidget.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"

#include "UI/MainMenu/BNSessionList.h"

UBNMainMenuWidget::UBNMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// WBP_SessionList를 찾아서 저장
	ConstructorHelpers::FClassFinder<UUserWidget> SessionListBPClass(TEXT("/Game/UI/MainMenu/WBP_SessionList"));
	if (SessionListBPClass.Succeeded())
	{
		SessionListClass = SessionListBPClass.Class;
	}
}

bool UBNMainMenuWidget::Initialize()
{
	bool SuperSuccess = Super::Initialize();
	if (!SuperSuccess) return false;

	// 버튼 클릭 델리게이트 바인딩
	if (Btn_Host == nullptr) return false;
	Btn_Host->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedHost);

	if (Btn_Host_As == nullptr) return false;
	Btn_Host_As->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedHostAs);

	if (Btn_Join == nullptr) return false;
	Btn_Join->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoin);

	if (Btn_Join_To == nullptr) return false;
	Btn_Join_To->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoinTo);
	
	if (Btn_Back == nullptr) return false;
	Btn_Back->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedBack);

	if (Btn_Cancel == nullptr) return false;
	Btn_Cancel->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedCancel);
	
	if (Btn_Quit == nullptr) return false;
	Btn_Quit->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedQuit);
	
	return true;
}

void UBNMainMenuWidget::SetMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface)
{
	MainMenuInterface = NewMainMenuInterface;
}

void UBNMainMenuWidget::SetSessionList(const TArray<FSessionData>& SessionDataList)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	// 위젯 생성 전, 이전 결과 초기화
	SessionList->ClearChildren();

	uint32 Index = 0;
	for (const FSessionData& SessionData : SessionDataList)
	{
		auto Session = CreateWidget<UBNSessionList>(World, SessionListClass);
		if (Session == nullptr) return;

		Session->SessionName->SetText(FText::FromString(SessionData.SessionName));
		Session->HostUserName->SetText(FText::FromString(SessionData.HostUserName));
		Session->PlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), SessionData.CurrentPlayer, SessionData.MaxPlayer)));
		
		Session->Setup(this, Index);
		SessionList->AddChild(Session);
		
		++Index;
	}
}

void UBNMainMenuWidget::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateSessionList();
}

void UBNMainMenuWidget::UpdateSessionList()
{
	for (int32 i = 0; i < SessionList->GetChildrenCount(); ++i)
	{
		auto Session = Cast<UBNSessionList>(SessionList->GetChildAt(i));
		if (Session == nullptr) return;

		Session->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
	}
}

void UBNMainMenuWidget::OpenMenu()
{
	// 메인 메뉴 화면에 띄우기
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr) return;

	// InputMode 설정 (UIOnly, Mouse 화면 밖에 벗어날 수 있게, 마우스 커서 보이게)
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UBNMainMenuWidget::CloseMenu()
{
	// 메인 메뉴 화면에서 지우기
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


void UBNMainMenuWidget::OnClickedHost()
{
	if (MenuSwitcher == nullptr) return;
	if (HostMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UBNMainMenuWidget::OnClickedHostAs()
{
	if (MainMenuInterface == nullptr) return;
	if (SessionHostName == nullptr) return;

	FString SessionName = SessionHostName->GetText().ToString();
	MainMenuInterface->Host(SessionName);
}

void UBNMainMenuWidget::OnClickedJoin()
{
	if (MainMenuInterface == nullptr) return;
	if (MenuSwitcher == nullptr) return;
	if (JoinMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(JoinMenu);
	MainMenuInterface->RefreshSessionList();
}

void UBNMainMenuWidget::OnClickedJoinTo()
{
	if (SelectedIndex.IsSet() && MainMenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index: %d"), SelectedIndex.GetValue());
		MainMenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Selected Index"));
	}
}

void UBNMainMenuWidget::OnClickedBack()
{
	if (MenuSwitcher == nullptr) return;
	if (MainMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBNMainMenuWidget::OnClickedCancel()
{
	if (MenuSwitcher == nullptr) return;
	if (MainMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBNMainMenuWidget::OnClickedQuit()
{
	if (MainMenuInterface == nullptr) return;

	MainMenuInterface->Quit();	
}
