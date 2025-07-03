// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BNBaseItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EBNItemUseType : uint8
{
	UseImmediately UMETA(DisplayName="즉시 사용 아이템"),
	StoreAndUseLater UMETA(DisplayName="보관 가능 아이템")
};

USTRUCT(BlueprintType)
struct FBNItemInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemEffectTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBNItemUseType UseType;
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNBaseItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBNItemInfo> ItemInfoList;

	const FBNItemInfo* FindItemInfoByID(FName InItemID) const;
};
