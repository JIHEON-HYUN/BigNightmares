// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryComponent.h"

#include "GameFramework/GameSession.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
	
}

void UInventoryComponent::AddItem(FName ItemID)
{
	if (ItemID.IsNone())
	{
		return;	
	}

	if (!ItemDataAsset || !ItemDataAsset->FindItemInfoByID(ItemID))
	{
		return;
	}
		
	int32& Count = OwnedItemID.FindOrAdd(ItemID);
	++Count;
}

void UInventoryComponent::RemoveItem(FName ItemID)
{
	if (ItemID.IsNone())
	{
		return;	
	}

	if (!ItemDataAsset || !ItemDataAsset->FindItemInfoByID(ItemID))
	{
		return;
	}

	int32* CountPtr = OwnedItemID.Find(ItemID);
	if (!CountPtr)
	{
		return;
	}

	--(*CountPtr);
	
	if (*CountPtr <= 0)
	{
		OwnedItemID.Remove(ItemID);
	}	
}

void UInventoryComponent::UsingItem(FName ItemID)
{
	
}

const FBNItemInfo* UInventoryComponent::GetItemInfo(FName ItemID) const
{
	if (!ItemDataAsset) return nullptr;

	for (const FBNItemInfo& Info : ItemDataAsset->ItemInfoList)
	{
		if (Info.ItemID == ItemID)
			return &Info;
	}
	return nullptr;
}


