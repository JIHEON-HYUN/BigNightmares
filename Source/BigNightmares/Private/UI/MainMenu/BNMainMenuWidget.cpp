// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BNMainMenuWidget.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

#include "UI/MainMenu/BNSessionList.h"

UBNMainMenuWidget::UBNMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> SessionListBPClass(TEXT("/Game/UI/MainMenu/WBP_SessionList"));
	if (SessionListBPClass.Succeeded())
	{
		SessionListClass = SessionListBPClass.Class;
	}
}

void UBNMainMenuWidget::SetIBNMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface)
{
	MainMenuInterface = NewMainMenuInterface;
}

void UBNMainMenuWidget::SetSessionList(TArray<FString> SessionNames)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	SessionList->ClearChildren();
	
	for (const FString& SessionName : SessionNames)
	{
		UBNSessionList* Session = CreateWidget<UBNSessionList>(World, SessionListClass);
		if (Session == nullptr) return;

		Session->SessionName->SetText(FText::FromString(SessionName));

		SessionList->AddChild(Session);
	}
}

bool UBNMainMenuWidget::Initialize()
{
	bool SuperSuccess = Super::Initialize();
	if (!SuperSuccess) return false;

	// 버튼 클릭 델리게이트 바인딩
	if (Btn_Host == nullptr) return false;
	Btn_Host->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedHost);

	if (Btn_Join == nullptr) return false;
	Btn_Join->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoin);

	if (Btn_Cancel == nullptr) return false;
	Btn_Cancel->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedCancel);

	if (Btn_Join_To == nullptr) return false;
	Btn_Join_To->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoinTo);

	if (Btn_Quit == nullptr) return false;
	Btn_Quit->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedQuit);
	
	return true;
}

void UBNMainMenuWidget::OnClickedHost()
{
	if (MainMenuInterface == nullptr) return;
	MainMenuInterface->Host();
}

void UBNMainMenuWidget::OnClickedJoin()
{
	if (MainMenuInterface == nullptr) return;
	if (JoinMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(JoinMenu);
	MainMenuInterface->RefreshSessionList();
}

void UBNMainMenuWidget::OnClickedCancel()
{
	if (MainMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBNMainMenuWidget::OnClickedJoinTo()
{
	if (MainMenuInterface == nullptr) return;

	MainMenuInterface->Join();
}

void UBNMainMenuWidget::OnClickedQuit()
{
	if (MainMenuInterface == nullptr) return;

	MainMenuInterface->Quit();	
}

void UBNMainMenuWidget::Setup()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr) return;
	
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UBNMainMenuWidget::CloseMenu()
{
	this->RemoveFromViewport();
	
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController == nullptr) return;
	
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}

