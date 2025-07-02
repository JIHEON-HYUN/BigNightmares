// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BNMainMenuWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"

void UBNMainMenuWidget::SetIBNMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface)
{
	MainMenuInterface = NewMainMenuInterface;
}

bool UBNMainMenuWidget::Initialize()
{
	bool SuperSuccess = Super::Initialize();
	if (!SuperSuccess) return false;

	if (Btn_Host == nullptr) return false;
	Btn_Host->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedHost);

	if (Btn_Join == nullptr) return false;
	Btn_Join->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoin);

	if (Btn_Cancel == nullptr) return false;
	Btn_Cancel->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedCancel);

	if (Btn_Join_To == nullptr) return false;
	Btn_Join_To->OnClicked.AddDynamic(this, &UBNMainMenuWidget::OnClickedJoinTo);
	
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
}

void UBNMainMenuWidget::OnClickedCancel()
{
	if (MainMenuInterface == nullptr) return;
	if (MainMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBNMainMenuWidget::OnClickedJoinTo()
{
	if (MainMenuInterface == nullptr) return;
	if (IPAddressBox == nullptr) return;

	MainMenuInterface->Join(IPAddressBox->GetText().ToString());
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

void UBNMainMenuWidget::QuitGame()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;
	
	APlayerController* PlayerController = World->GetFirstPlayerController();
	
	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, false);
}
