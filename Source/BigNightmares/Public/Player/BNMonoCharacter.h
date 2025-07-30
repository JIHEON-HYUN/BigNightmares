// BNMonoCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/BNBaseCharacter.h"
#include "Player/BNPlayerRole.h"
#include "BNMonoCharacter.generated.h"

struct FOnAttributeChangeData;
class UBNBaseItemDataAsset;
class UInventoryComponent;
class UDataAsset_InputConfig_Player;
class USpringArmComponent;
class UCameraComponent;
class USpotLightComponent; 
class UAnimMontage; 
class IInteractionInterface; 
class UInputAction; 

struct FInputActionValue;
/**
 * */
UCLASS()
class BIGNIGHTMARES_API ABNMonoCharacter : public ABNBaseCharacter
{
	GENERATED_BODY()

	ABNMonoCharacter();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Character|Combat")
	void FreezePlayer(AActor* LookAtTarget);

	void Die(AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable, Category = "Character|Combat")
	void HandleImmediateDeath(AActor* DamageCauser);

	void SetInteractableActor(AActor* Actor);
	
	void TriggerGuaranteedDeath();
	void HandleLethalHit();

	void SetOverlappedInteractable(AActor* Interactable);
	void ClearOverlappedInteractable(AActor* Interactable);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleDeath(AActor* DamageCauser);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsImmobilized = false;

	void EndImmobilization();

	FTimerHandle ImmobilizeTimerHandle;
	
	float OriginalMaxWalkSpeed;

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_UseItem(const FInputActionValue& InputActionValue);

	void Input_Interact();

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* TargetActor);
	
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

#pragma region Flashlight
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Flashlight", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpotLightComponent> FlashlightComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flashlight")
	float FlashlightBeamRadius = 100.0f;
	

private:
	TWeakObjectPtr<AActor> LastLitActor;
#pragma endregion
	
public:
	
	void OnMoveSpeedChanged(const FOnAttributeChangeData Data);

	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float NewSpeed);

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Role")
	EPlayerRole PlayerRole = EPlayerRole::None;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> OverlappedInteractableActor;
	
public:
	template<typename T>
	T* GetAttributeSet() const
	{
		return Cast<T>(MonoCharacterAttributeSet.Get());
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
#pragma region Client
public:
	UFUNCTION(Client, Reliable)
	void Client_ApplyMoveSpeed(float NewSpeed);

#pragma endregion
};
