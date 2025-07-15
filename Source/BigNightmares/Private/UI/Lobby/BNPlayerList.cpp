// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/BNPlayerList.h"

#include "Components/Image.h"
#include "UI/Lobby/BNLobbyWidget.h"

void UBNPlayerList::Setup(class UBNLobbyWidget* NewParent)
{
	Parent = NewParent;
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
