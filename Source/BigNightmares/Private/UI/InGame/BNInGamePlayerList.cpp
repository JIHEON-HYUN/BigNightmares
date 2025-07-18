// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNInGamePlayerList.h"

#include "Components/TextBlock.h"

void UBNInGamePlayerList::Setup(FString NewSteamID, bool NewStatusAlive)
{
	if (SteamID == nullptr) return;
	if (StatusAlive == nullptr) return;

	bStatusAlive = NewStatusAlive;
	
	SteamID->SetText(FText::FromString(NewSteamID));
	if (bStatusAlive)
	{
		StatusAlive->SetText(FText::FromString("Alive"));
	}
	else
	{
		StatusAlive->SetText(FText::FromString("Dead"));
	}
}

void UBNInGamePlayerList::ChangeStatusAlive()
{
	if (StatusAlive == nullptr) return;
	
	if (bStatusAlive)
	{
		StatusAlive->SetText(FText::FromString("Dead"));
	}
	else
	{
		StatusAlive->SetText(FText::FromString("Alive"));
	}

	bStatusAlive = !bStatusAlive;
}

const FString UBNInGamePlayerList::GetSteamID()
{
	if (SteamID == nullptr) return TEXT("SteamID is nullptr");
	
	return SteamID->GetText().ToString();
}
