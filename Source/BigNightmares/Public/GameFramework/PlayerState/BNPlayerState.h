// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNTarotCardAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "UI/InGame/InventoryInterface.h"
#include "BNPlayerState.generated.h"

class UInventoryComponent;
class UBNTarotCardAttributeSet;
class UBNBaseAttributeSet;
class UBNBaseAbilitySystemComponent;

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	Prey,
	Resident
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNPlayerState : public APlayerState, public IAbilitySystemInterface, public IInventoryInterface
{
	GENERATED_BODY()

public:
	ABNPlayerState();

#pragma region InGame

private:
	EPlayerType PlayerType = EPlayerType::Prey;

public:
	void SetPlayerType(EPlayerType NewType);
	
#pragma endregion InGame

#pragma region Abilities

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// FORCEINLINE UBNBaseAbilitySystemComponent* GetBNBaseAbilitySystemComponent() const { return BNBaseAbilitySystemComponent.Get(); }
	// FORCEINLINE UBNTarotCardAttributeSet* GetBNBaseAttributeSet() const {return Cast<UBNTarotCardAttributeSet>(BNBaseAttributeSet.Get());}

	UFUNCTION(BlueprintPure)
	UBNBaseAbilitySystemComponent* GetBNBaseAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure)
	UBNBaseAttributeSet* GetBNBaseAttributeSet() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess=true))
	TObjectPtr<UBNBaseAbilitySystemComponent> BNBaseAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess=true))
	TObjectPtr<UBNBaseAttributeSet> BNBaseAttributeSet;

public:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	template<typename T>
	T* CreateAttributeSet(const FName& Name)
	{
		static_assert(std::is_base_of<UBNBaseAttributeSet, T>::value, "Create : T must derive from UBNBaseAttributeSet");
		return CreateDefaultSubobject<T>(Name);
	}

	//해당 방식은 MonoCharacter에서 인식을 못하는 오류를 어디서 고쳐야할지 감이 안잡혀서 보류 (사용 안하는중)
	template<typename T>
	T* GetAttributeSet() const
	{
		static_assert(std::is_base_of<UBNBaseAttributeSet, T>::value, "Getter : T must derive from UBNBaseAttributeSet");
		return Cast<T>(BNBaseAttributeSet.Get());
	}
#pragma endregion Abilities

#pragma region Inventory

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
	/* Implement Inventory interface */
	virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true), Replicated)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	
#pragma endregion Inventory
};
