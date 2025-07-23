// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "UI/InGame/InventoryInterface.h"
#include "UI/Lobby/BNLobbyInterface.h"
#include "BNPlayerController.generated.h"

class UBNMission1Widget;
class UVerticalTimingGaugeComponent;
enum class EVerticalGaugeResult : uint8;
class UImage;
class UBorder;
class UBNInGameWidget;
struct FLobbyPlayerData;

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
	virtual void BeginPlay() override;

#pragma region Lobby
	
	TSubclassOf<class UUserWidget> LobbyClass;
	
	UPROPERTY()
	class UBNLobbyWidget* LobbyWidget;
	
	UFUNCTION(BlueprintCallable)
	virtual void LoadLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void OpenLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseLobbyMenu() override;
	UFUNCTION(BlueprintCallable)
	virtual void ReturnToMainMenu() override;
	UFUNCTION(Server, Reliable)
	virtual void ChangePlayerReadyState() override;

	// LobbyPlayerDataList가 업데이트되면 콜백
	UFUNCTION()
	void OnLobbyPlayerUpdated_Handler(const TArray<FLobbyPlayerData>& UpdatedList);

#pragma endregion Lobby

#pragma region Inventory
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UBNInventoryWidgetController* GetBNInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TObjectPtr<UBNInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UBNInventoryWidgetController> BNInventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBNSystemWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Widgets")
	TSubclassOf<UBNSystemWidget> InventoryWidgetClass;

#pragma endregion

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

#pragma region Missions1
public:
	UPROPERTY(EditDefaultsOnly, Category="Timing Gauge | UI")
	TSubclassOf<UBNMission1Widget> Mission1WidgetClass;

	//현재 게이지 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UBNMission1Widget> Mission1WidgetInstance;
	
	UPROPERTY(transient)
	TWeakObjectPtr<UVerticalTimingGaugeComponent> ActiveGaugeComponent;
	
	// UI활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Timing Gauge | State")
	bool bIsGaugeActive;
	
	float CachedGaugeHeight;	//게이지 바 전체 높이 캐시
	float CachedPointerHeight;	//커서 높이 캐시

	//클라이언트가 게이지 결과를 서버에 보고하는 RPC (원래는 서버가 결과도 검사하고 다시 클라에 보내야하지만 구현하다가 터짐..)
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReportGaugeInput(FGuid InGaugeID, float SmoothedGaugeValue);
	
	//클라이언트에서 게이지 실제로 시작
	UFUNCTION(Client, Reliable)
	void Client_ShowMission1GaugeUI(UVerticalTimingGaugeComponent* InGaugeComponent, int32 MaxLife, int32 RequiredSuccess);
	
	// 클라이언트에서 게이지 UI를 실제 종료 (서버로 부터 호출)
	UFUNCTION(Client, Reliable)
	void Client_EndGaugeUI(EVerticalGaugeResult Result);

	UFUNCTION(Server, Reliable, WithValidation) // WithValidation 데이터 유효화 검사
	void Server_NotifyGaugeFinished(FGuid InGaugeID, EVerticalGaugeResult Result);

#pragma endregion
};
