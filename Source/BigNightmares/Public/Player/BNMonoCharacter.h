// Fill out your copyright notice in the Description page of Project Settings.

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
class USpotLightComponent; // 추가

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNMonoCharacter : public ABNBaseCharacter
{
	GENERATED_BODY()

	ABNMonoCharacter();

	// 추가 ~
public:
	// [손전등 기능 추가] Tick 함수를 오버라이드하여 매 프레임 로직을 처리합니다.
	virtual void Tick(float DeltaTime) override;
	// ~ 추가
	
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

	// 추가 ~
	// [손전등 기능 추가] 시작: 손전등 관련 변수들을 추가합니다.
#pragma region Flashlight
protected:
	// 손전등 역할을 하는 스포트라이트 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Flashlight", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpotLightComponent> FlashlightComponent;

	// [핵심 추가] 손전등 빛의 판정 범위를 나타내는 구체의 반지름입니다.
	// 이 값을 블루프린트에서 조절하여 판정 크기를 바꿀 수 있습니다.
	// [핵심 추가] 손전등 빛의 판정 범위를 나타내는 구체의 반지름입니다.
	// 이 값을 블루프린트에서 조절하여 판정 크기를 바꿀 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Flashlight")
	float FlashlightBeamRadius = 100.0f;

private:
	// 이전에 빛을 비췄던 액터를 기억하기 위한 변수입니다.
	TWeakObjectPtr<AActor> LastLitActor;
#pragma endregion
	// [손전등 기능 추가] 끝
	// ~ 추가
	
public:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	// TObjectPtr<UInventoryComponent> InventoryComponent;
	
	void OnMoveSpeedChanged(const FOnAttributeChangeData Data);

	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float NewSpeed);

	// 추가 ~
	// PlayerRole 변수를 public 영역으로 옮겨서 GameMode가 접근할 수 있도록 합니다.
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Role")
	EPlayerRole PlayerRole = EPlayerRole::None;
	// ~ 추가
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	template<typename T>
	T* GetAttributeSet() const
	{
		return Cast<T>(MonoCharacterAttributeSet.Get());
	}

	// 추가 ~
	// 네트워크를 통해 변수를 복제하기 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ~ 추가
	
#pragma region Client
public:
	UFUNCTION(Client, Reliable)
	void Client_ApplyMoveSpeed(float NewSpeed);
#pragma endregion
};
