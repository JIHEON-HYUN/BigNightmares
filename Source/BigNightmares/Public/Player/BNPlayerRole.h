// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BNPlayerRole.generated.h"

// 플레이어의 역할을 구분하기 위한 열거형(Enum)입니다.
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	// 손전등을 가진 플레이어
	FlashlightHolder,

	// 열쇠를 가진 플레이어
	KeyHolder,

	// 역할이 아직 정해지지 않음
	None
};
