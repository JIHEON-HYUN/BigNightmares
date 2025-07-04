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

	//TODO : VisibleAnywhere로 수정해함 Debug를 위해서 EditDefaultsOnly로 바꿔둠
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	TMap<FName, int32> OwnedItemID;

	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemID);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(FName ItemID);

	UFUNCTION(BlueprintCallable)
	void UsingItem(FName ItemID);

	const FBNItemInfo* GetItemInfo(FName ItemID) const;
};
