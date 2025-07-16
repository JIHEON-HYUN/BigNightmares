// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNLobbyInterface.h"
#include "BNLobbyWidget.generated.h"

struct FLobbyPlayerData;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// IBNLobbyInterface를 상속받은 게임 인스턴스의 기능을 사용하기 위한 변수
	IBNLobbyInterface* LobbyInterface;
	// WBP_PlayerList를 저장하기 위한 변수
	TSubclassOf<class UUserWidget> PlayerListClass;

public:
	UBNLobbyWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual bool Initialize() override;

public:
	// LobbyInterface에 의존성 주입
	void SetLobbyInterface(IBNLobbyInterface* NewLobbyInterface);
	void SetPlayerList(const TArray<FLobbyPlayerData>& PlayerDataList);
	
	void OpenMenu();
	void CloseMenu();

	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Ready;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UPanelWidget* PlayerListBox;

	// Callback Func
	UFUNCTION()
	void OnClickedExit();
	UFUNCTION()
	void OnClickedReady();
};
