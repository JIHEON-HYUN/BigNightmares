// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNInGameWidget.generated.h"

struct FInGamePlayerData;
class IBNInGameInterface;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNInGameWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	// IBNInGameInterface를 상속받은 게임 인스턴스의 기능을 사용하기 위한 변수
	IBNInGameInterface* InGameInterface;
	// WBP_InGamePlayerList를 저장하기 위한 변수
	TSubclassOf<class UUserWidget> InGamePlayerListClass;
	
public:
	UBNInGameWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool Initialize() override;

public:
	// InGameInterface에 의존성 주입
	void SetInGameInterface(IBNInGameInterface* NewInGameInterface);
	void SetPlayerList(const TArray<FInGamePlayerData>& PlayerDataList);

	void OpenMenu();
	void CloseMenu();

	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PrivateMissionStatus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* CooperativeMissionStatus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SacrificeMissionStatus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* AssignableMissionStatus;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UPanelWidget* PlayerListBox;

	// Callback Func
	UFUNCTION()
	void OnClickedExit();
};
