// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNSystemWidget.h"

void UBNSystemWidget::SetWidgetController(UBNWidgetController* InWidgetController)
{
	BNWidgetController = InWidgetController;
	OnWidgetControllerSet();
}
