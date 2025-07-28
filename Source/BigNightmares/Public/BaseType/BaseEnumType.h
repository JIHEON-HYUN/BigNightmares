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

UENUM(BlueprintType)
enum class EMissionResult : uint8
{
	Success UMETA(DisplayName = "Success"), //게임결과 실패 성공
	Failure UMETA(DisplayName = "Failure")
};

//Assignable의 이동방향 열거형
UENUM(BlueprintType)
enum class EAssignableMissionMovementDirection : uint8
{
	Forward,
	Backward,
	Stop
};