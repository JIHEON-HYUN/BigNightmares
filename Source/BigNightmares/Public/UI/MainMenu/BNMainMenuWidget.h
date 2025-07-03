// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNMainMenuInterface.h"
#include "BNMainMenuWidget.generated.h"

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
	
	// MainMenuInterface에 의존성 주입
	void SetIBNMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface);

	void SetSessionList(TArray<FString> SessionNames);

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

public:
	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Host;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join_To;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Quit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* MainMenu;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UPanelWidget* SessionList;

	TOptional<uint32> SelectedIndex;

	void Setup();
	void CloseMenu();
	
	// Callback Func
	UFUNCTION()
	void OnClickedHost();
	UFUNCTION()
	void OnClickedJoin();
	UFUNCTION()
	void OnClickedCancel();
	UFUNCTION()
	void OnClickedJoinTo();
	UFUNCTION()
	void OnClickedQuit();
};
