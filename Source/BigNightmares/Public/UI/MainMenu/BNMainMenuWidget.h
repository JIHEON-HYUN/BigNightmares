// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNMainMenuInterface.h"
#include "BNMainMenuWidget.generated.h"

USTRUCT()
struct FSessionData
{
	GENERATED_BODY()

	FString SessionName;
	uint16 CurrentPlayer;
	uint16 MaxPlayer;
	FString HostUserName;
};

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// IBNMainMenuInterface를 상속받은 게임 인스턴스의 기능을 사용하기 위한 변수
	IBNMainMenuInterface* MainMenuInterface;
	// WBP_SessionList를 저장하기 위한 변수
	TSubclassOf<class UUserWidget> SessionListClass;
	
public:
	UBNMainMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual bool Initialize() override;

public:
	// MainMenuInterface에 의존성 주입
	void SetMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface);
	
	void SetSessionList(const TArray<FSessionData>& SessionDataList);
	void SelectIndex(uint32 Index);
	void UpdateSessionList();
	
	void OpenMenu();
	void CloseMenu();

public:
	// Widget - Button
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Host;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Host_As;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join_To;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Back;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Quit;

	// Widget - MenuBox
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* HostMenu;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableText* SessionHostName;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UPanelWidget* SessionList;

	// JoinMenu에서 선택한 세션 인덱스 저장
	TOptional<uint32> SelectedIndex;
	
	// Callback Func
	UFUNCTION()
	void OnClickedHost();
	UFUNCTION()
	void OnClickedHostAs();
	UFUNCTION()
	void OnClickedJoin();
	UFUNCTION()
	void OnClickedJoinTo();
	UFUNCTION()
	void OnClickedBack();
	UFUNCTION()
	void OnClickedCancel();
	UFUNCTION()
	void OnClickedQuit();
};
