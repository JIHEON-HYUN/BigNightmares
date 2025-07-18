// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "Library/BNAbilitySystemLibrary.h"

bool FPackagedInventory::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SafeNetSerializeTArray_WithNetSerialize<100>(Ar, ItemTags, Map);
	SafeNetSerializeTArray_Default<100>(Ar, ItemQuantities);
	bOutSuccess = true;
	return true;
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CachedInventory);
}

/*
 *   YouTube Inventory System
 */

void UInventoryComponent::AddItemByTag(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();

#pragma region Debug IsValid(Owner)
	if (!IsValid(Owner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Owner :  %s"), *Owner->GetName()));
		return;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald,
		FString::Printf(TEXT("Owner :  %s"), *Owner->GetName()));
	}
#pragma endregion
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Owner Role: %s"),
		*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), Owner->GetLocalRole())));
	
	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag, NumItems);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald,
		FString::Printf(TEXT("ServerAddItem In")));
		return;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("ServerAddItem Out")));
	}


	if (InventoryTagMap.Contains(ItemTag))
	{
		InventoryTagMap[ItemTag] += NumItems;
	}
	else
	{
		InventoryTagMap.Emplace(ItemTag, NumItems);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Server Item Added To Inventory %s, qty : %d"), *ItemTag.ToString(), NumItems));

	PackageInventory(CachedInventory);
	InventoryPackagedDelegate.Broadcast(CachedInventory);
}


void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItemByTag(ItemTag, NumItems);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("ServerAddItem_Implementation Call")));
}

void UInventoryComponent::PackageInventory(FPackagedInventory& OutInventory)
{
	OutInventory.ItemTags.Empty();
	OutInventory.ItemQuantities.Empty();

	for (const auto& Pair : InventoryTagMap)
	{
		if (Pair.Value > 0)
		{
			OutInventory.ItemTags.Add(Pair.Key);
		 	OutInventory.ItemQuantities.Add(Pair.Value);
		}
	}
}

void UInventoryComponent::ReconstructInventoryMap(const FPackagedInventory& Inventory)
{
	InventoryTagMap.Empty();
	for (int32 i = 0; i<Inventory.ItemTags.Num(); ++i)
	{
		InventoryTagMap.Emplace(Inventory.ItemTags[i], Inventory.ItemQuantities[i]);

		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,
			FString::Printf(TEXT("Tag Added : %s // Quantity Added : %d"), *Inventory.ItemTags[i].ToString(), Inventory.ItemQuantities[i]));
	}
}

void UInventoryComponent::OnRep_CachedInventory()
{
	ReconstructInventoryMap(CachedInventory);
	InventoryPackagedDelegate.Broadcast(CachedInventory);
}

void UInventoryComponent::UseItemByTag(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	UE_LOG(LogTemp, Warning, TEXT("UseItemByTag Point 1"));
	if (!Owner->HasAuthority())
	{
		ServerUseItem(ItemTag, NumItems);
		UE_LOG(LogTemp, Warning, TEXT("UseItemByTag Point 2"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("UseItemByTag Point 3"));
	FMasterItemDefinition Item = GetItemDefinitionByTag(ItemTag);

	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItemByTag Point 4"));
		if (IsValid(Item.GameEffectProps.ItemEffectClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("UseItemByTag Point 5"));
			FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.GameEffectProps.ItemEffectClass,
				Item.GameEffectProps.ItemEffectLevel, ContextHandle);
			OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			AddItemByTag(ItemTag, -1);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,
				FString::Printf(TEXT("Server Item Used : %s"), *Item.ItemTag.ToString()));
		}
	}
}

void UInventoryComponent::ServerUseItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	UseItemByTag(ItemTag, NumItems);
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinitions, TEXT("No Inventory Definitions Inside  Component %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinitions->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			return *UBNAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
		}
	}

	return FMasterItemDefinition();
}

TMap<FGameplayTag, int32> UInventoryComponent::GetInventoryTagMap()
{
	return InventoryTagMap;
}

