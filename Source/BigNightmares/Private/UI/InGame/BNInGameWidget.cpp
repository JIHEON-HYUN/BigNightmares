// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNInGameWidget.h"

#include "Components/Image.h"
#include "DataAsset/Item/BNBaseItemDataAsset.h"

void UBNInGameWidget::InitWithItemData(const UBNBaseItemDataAsset* InItemData)
{
	if (!InItemData || !Image_TarotCard) return;

	if (InItemData->ItemInfoList[0].ItemID == FName("0"))
	{
		Image_TarotCard->SetBrushFromTexture(InItemData->ItemInfoList[0].ItemIcon);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Not Found"));
	}
}
