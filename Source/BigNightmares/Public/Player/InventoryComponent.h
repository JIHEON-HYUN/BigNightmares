// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TarotCard/ItemTypes.h"
#include "DataAsset/Item/BNBaseItemDataAsset.h"
#include "InventoryComponent.generated.h"

USTRUCT()
struct FPackagedInventory
{
	GENERATED_BODY()

	virtual ~FPackagedInventory() = default;
	
	UPROPERTY()
	TArray<FGameplayTag> ItemTags;

	UPROPERTY()
	TArray<int32> ItemQuantities;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};
template<>
struct TStructOpsTypeTraits<FPackagedInventory> : TStructOpsTypeTraitsBase2<FPackagedInventory>
{
	enum
	{
		WithNetSerializer = true
	};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryPackagedSignature, const FPackagedInventory& /* InventoryContents*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIGNIGHTMARES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	FInventoryPackagedSignature InventoryPackagedDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region YoutubeInven

public:
	UFUNCTION(BlueprintCallable)
	void AddItemByTag(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItemByTag(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TMap<FGameplayTag, int32> GetInventoryTagMap();
	
	void ReconstructInventoryMap(const FPackagedInventory& Inventory);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TMap<FGameplayTag, int32> InventoryTagMap;

	UPROPERTY(ReplicatedUsing=OnRep_CachedInventory)
	FPackagedInventory CachedInventory;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBNBaseItemDataAsset> InventoryDefinitions;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FGameplayTag& ItemTag, int32 NumItems);

	void PackageInventory(FPackagedInventory& OutInventory);

	UFUNCTION()
	void OnRep_CachedInventory();
#pragma endregion

#pragma region DebugValue

public:
	UPROPERTY()
	int32 CurrentAddItemRetries = 0;

	UPROPERTY()
	int32 MaxAddItemRetries = 20;

	FTimerHandle AddItemRetryTimerHandle;
#pragma endregion
};
