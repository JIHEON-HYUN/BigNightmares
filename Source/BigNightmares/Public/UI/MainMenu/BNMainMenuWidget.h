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
	IBNMainMenuInterface* MainMenuInterface;
	
public:
	void SetIBNMainMenuInterface(IBNMainMenuInterface* NewMainMenuInterface);

protected:
	virtual bool Initialize() override;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Host;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Join_To;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* MainMenu;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* IPAddressBox;

	UFUNCTION()
	void OnClickedHost();
	UFUNCTION()
	void OnClickedJoin();
	UFUNCTION()
	void OnClickedCancel();
	UFUNCTION()
	void OnClickedJoinTo();

	void Setup();
	void CloseMenu();
	void QuitGame();
};
