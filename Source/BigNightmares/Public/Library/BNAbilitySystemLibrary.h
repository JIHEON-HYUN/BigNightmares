// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BNAbilitySystemLibrary.generated.h"

class UBNMonoCharacterDataAsset;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static UBNMonoCharacterDataAsset* GetMonoCharacterDataAsset(const UObject* WorldContextObject);

	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag);
};

template <typename T>
T* UBNAbilitySystemLibrary::GetDataTableRowByTag(UDataTable* DataTable, FGameplayTag Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), FString(""));
}
