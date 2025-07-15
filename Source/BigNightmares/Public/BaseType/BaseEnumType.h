#pragma once

UENUM()
enum class EBaseSuccessType: uint8
{
	Success,
	Failed
};

UENUM(BlueprintType)
enum class EVerticalGaugeResult : uint8
{
	EVGR_Success UMETA(DisplayName="Success"), // 녹색 영역 내에서 성공
	EVGR_Fail UMETA(DisplayName="Fail"),
};