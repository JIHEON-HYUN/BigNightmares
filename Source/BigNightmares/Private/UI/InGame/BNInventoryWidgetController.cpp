// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNInventoryWidgetController.h"
#include "Player/InventoryComponent.h"
#include "UI/InGame/InventoryInterface.h"

void UBNInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UBNInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	UE_LOG(LogTemp, Error, TEXT("%s"), *OwningInventory.GetName());

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryPackagedDelegate.AddLambda(
			[this] (const FPackagedInventory& InventoryContents)
			{
				UpdateInventory(InventoryContents);
			}
		);
	}
}

void UBNInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		BroadcastInventoryContents();
	}
}

void UBNInventoryWidgetController::UpdateInventory(const FPackagedInventory& InventoryContents)
{
	if (IsValid(OwningInventory))
	{
		OwningInventory->ReconstructInventoryMap(InventoryContents);

		BroadcastInventoryContents();
	}
}

void UBNInventoryWidgetController::BroadcastInventoryContents()
{
	if (IsValid(OwningInventory))
	{
		TMap<FGameplayTag, int32> LocalInventoryMap = OwningInventory->GetInventoryTagMap();

		ScrollBoxResetDelegate.Broadcast();

		for (const auto& Pair : LocalInventoryMap)
		{
			FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Pair.Key);
			Item.ItemQuantity = Pair.Value;

			InventoryItemDelegate.Broadcast(Item);
		}

		InventoryBroadcastComplete.Broadcast();
	}
}
