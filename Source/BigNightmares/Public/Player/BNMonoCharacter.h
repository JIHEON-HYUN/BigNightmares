// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/BNBaseCharacter.h"
#include "BNMonoCharacter.generated.h"

struct FOnAttributeChangeData;
class UBNBaseItemDataAsset;
class UInventoryComponent;
class UDataAsset_InputConfig_Player;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNMonoCharacter : public ABNBaseCharacter
{
	GENERATED_BODY()

	ABNMonoCharacter();

protected:
	virtual void BeginPlay() override;
	
#pragma region Camera
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
#pragma  endregion

#pragma region Inputs
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CharacterData", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig_Player> InputConfigDataAsset;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);	
	void Input_UseItem(const FInputActionValue& InputActionValue);	
#pragma endregion

private:
#pragma region ASC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBNBaseAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBNBaseAttributeSet> MonoCharacterAttributeSet;

	UPROPERTY(EditAnywhere, Category="Custom Values|Character Info")
	FGameplayTag CharacterTag;
	
	UFUNCTION()
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void InitAbilityActorInfo();
	void InitClassDefaults();
	void BindCallbacksToDependencies();

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
#pragma endregion
	
public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	// TObjectPtr<UInventoryComponent> InventoryComponent;
	
	void OnMoveSpeedChanged(const FOnAttributeChangeData Data);

	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float NewSpeed);
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	template<typename T>
	T* GetAttributeSet() const
	{
		return Cast<T>(MonoCharacterAttributeSet.Get());
	}

#pragma region Client
public:
	UFUNCTION(Client, Reliable)
	void Client_ApplyMoveSpeed(float NewSpeed);
#pragma endregion
};
