// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/Item/BNBaseItemDataAsset.h"

const FBNItemInfo* UBNBaseItemDataAsset::FindItemInfoByID(FName InItemID) const
{
	return ItemInfoList.FindByPredicate([&](const FBNItemInfo& Info)
	{
		return Info.ItemID == InItemID;
	});
}
