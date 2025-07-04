// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BNBaseItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EBNCardEffectTrigger : uint8
{
	UseImmediately UMETA(DisplayName="즉시 사용 아이템"),
	StoreAndUseLater UMETA(DisplayName="보관 가능 아이템")
};

UENUM(BlueprintType)
enum class EGAUsage : uint8
{
	UseGA,
	SkipGA,
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
	
	//아이템 종류 (소모품, 장비, 기타)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemType;

	//즉시 사용 아이템, 보관 가능 아이템 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBNCardEffectTrigger UseType;

	//아이템의 GA사용 유무
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGAUsage GAUseType;

	//아이템의 효과 태그 (Effect_Item_TarotCard_MoveSpeed)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemEffectTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> ItemGameplayAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemGameplayEffect;

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
