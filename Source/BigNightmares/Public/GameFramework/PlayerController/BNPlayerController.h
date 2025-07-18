// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "UI/InGame/InventoryInterface.h"
#include "UI/Lobby/BNLobbyInterface.h"
#include "BNPlayerController.generated.h"

class UBNSystemWidget;
class UInventoryComponent;
/**
 * 
 */

UCLASS()
class BIGNIGHTMARES_API ABNPlayerController : public APlayerController, public IAbilitySystemInterface, public IBNLobbyInterface
{
	GENERATED_BODY()

public:
	ABNPlayerController();

	// LobbyWidget 기능
	UFUNCTION(BlueprintCallable)
	virtual void LoadLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void OpenLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void ReturnToMainMenu() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UBNInventoryWidgetController* GetBNInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	TSubclassOf<class UUserWidget> LobbyClass;
	class UBNLobbyWidget* LobbyWidget;
	
	UPROPERTY()
	TObjectPtr<UBNInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UBNInventoryWidgetController> BNInventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBNSystemWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UBNSystemWidget> InventoryWidgetClass;

#pragma region PlayerState_IsValid

public:
	virtual void OnRep_PlayerState() override;
	
	// RPC 호출이 가능한 상태인지 나타내는 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
	bool bCanCallServerRPC = false;
	
protected:
	void TryInitializeRpcReadiness();

	FTimerHandle InitHandle; //타이머 핸들, player controller가 초기화가 제대로 안됬다면 일정 시간 후 다시 반복하기 위함
	
#pragma endregion PlayerState_IsValid
};
