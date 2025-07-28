// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LightSensitive.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULightSensitive : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BIGNIGHTMARES_API ILightSensitive
{
	GENERATED_BODY()

public:
	// 이 함수는 플레이어의 플래시라이트 빛에 맞았을 때 호출됩니다.
	// bIsLit: true이면 빛에 맞은 상태, false이면 빛에서 벗어난 상태입니다.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Light Interaction")
	void OnHitByLight(bool bIsLit);
};
