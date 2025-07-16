// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/BNPlayerList.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Lobby/BNLobbyWidget.h"

void UBNPlayerList::Setup(FString NewSteamID, bool ReadyState)
{
	if (SteamID == nullptr) return;
	if (CheckReady == nullptr) return;
	
	SteamID->SetText(FText::FromString(NewSteamID));
	if (ReadyState)
	{
		CheckReady->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CheckReady->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBNPlayerList::ChangeReadyState()
{
	if (CheckReady == nullptr) return;

	if (CheckReady->IsVisible())
	{
		CheckReady->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CheckReady->SetVisibility(ESlateVisibility::Visible);
	}
}

const FString UBNPlayerList::GetSteamID()
{
	if (SteamID == nullptr) return TEXT("SteamID is nullptr");
	
	return SteamID->GetText().ToString();
}
