// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNSessionList.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNSessionList : public UUserWidget
{
	GENERATED_BODY()

public:
	// Widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* SessionName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* HostUserName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* PlayerCount;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Session;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

	void Setup(class UBNMainMenuWidget* NewParent, uint32 NewIndex);

	// Callback Func
	UFUNCTION()
	void OnClickedSession();

private:
	// 부모 메인 메뉴 위젯
	UPROPERTY()
	class UBNMainMenuWidget* Parent;

	// 선택할 세션의 인덱스
	uint32 Index;
};
