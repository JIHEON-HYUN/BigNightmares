// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNSystemWidget.generated.h"

class UBNWidgetController;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNSystemWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetWidgetController(UBNWidgetController* InWidgetController);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetControllerSet();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBNWidgetController> BNWidgetController;
	
};
