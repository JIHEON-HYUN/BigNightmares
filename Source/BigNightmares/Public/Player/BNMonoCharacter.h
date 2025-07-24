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
class UAnimMontage; // [추가] UAnimMontage 클래스 전방 선언
class IInteractionInterface; // [추가] 인터페이스 전방 선언
class UInputAction; // [수정] 이 줄을 추가하여 UInputAction 클래스를 전방 선언합니다.

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

	// 플레이어의 움직임을 멈추고, 지정된 대상을 바라보게 합니다.
	UFUNCTION(BlueprintCallable, Category = "Character|Combat")
	void FreezePlayer(AActor* LookAtTarget);

	// [추가] 몽타주 없이 즉시 래그돌 상태로 전환하고, 피격 방향으로 힘을 받습니다.
	UFUNCTION(BlueprintCallable, Category = "Character|Combat")
	void HandleImmediateDeath(AActor* DamageCauser);

	// [추가] 상호작용 가능한 액터를 설정하는 함수입니다. ChestActor가 이 함수를 호출할 것입니다.
	void SetInteractableActor(AActor* Actor);
	
	// [참고] 아래 함수들은 이제 이 특정 공격에서는 사용되지 않습니다.
	void TriggerGuaranteedDeath();
	void HandleLethalHit();

	// ChestActor가 호출할 함수들입니다.
	void SetOverlappedInteractable(AActor* Interactable);
	void ClearOverlappedInteractable(AActor* Interactable);
	
protected:
	virtual void BeginPlay() override;

	// [추가] 플레이어가 피격 당했을 때 재생할 몽타주입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	/** 캐릭터가 사망했는지 여부를 저장하는 변수입니다. 중복 사망 처리를 방지합니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	/** 캐릭터가 움직일 수 없는 상태인지 나타냅니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsImmobilized = false;

	/** 이동 불가 상태를 해제하는 함수입니다. */
	void EndImmobilization();

	/** 이동 불가 타이머를 위한 핸들입니다. */
	FTimerHandle ImmobilizeTimerHandle;
	
	/** 원래 이동 속도를 저장하기 위한 변수입니다. */
	float OriginalMaxWalkSpeed;

	// [추가] 사망 몽타주가 끝났을 때 호출될 콜백 함수입니다.
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

	// [추가] 상호작용 Input Action 에셋을 직접 담을 변수입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);	
	void Input_UseItem(const FInputActionValue& InputActionValue);

	// [추가] 상호작용 입력을 처리할 함수입니다.
	void Input_Interact();

	// [추가] 서버에 상호작용 요청을 보내는 함수입니다. (RPC)
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

private:
	// [수정] FocusedActor를 새로운 변수명으로 변경하고 역할을 명확히 합니다.
	// 이 변수는 현재 캐릭터가 겹쳐있는 상호작용 가능한 액터를 저장합니다.
	UPROPERTY()
	TWeakObjectPtr<AActor> OverlappedInteractableActor;
	
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
