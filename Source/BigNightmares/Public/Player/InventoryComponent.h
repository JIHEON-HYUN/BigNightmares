// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/Item/BNBaseItemDataAsset.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIGNIGHTMARES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UBNBaseItemDataAsset> ItemDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TMap<FName, int32> OwnedItemID;

	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemID);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(FName ItemID);
	
};
