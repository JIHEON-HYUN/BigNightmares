// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Mission/BaseMissionActor.h"
#include "MissionTimingGauge.generated.h"

enum class EVerticalGaugeResult : uint8;
enum class EMissionResult : uint8;
class ABNPlayerState;
class UVerticalTimingGaugeComponent;
class ABNPlayerController;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API AMissionTimingGauge : public ABaseMissionActor
{
	GENERATED_BODY()

public:
	AMissionTimingGauge();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing Gauge")
	TObjectPtr<UVerticalTimingGaugeComponent> TimingGaugeComponent;
	
protected:
	//현재 이 미니게임에 도전 중인 플레이어 (서버에서만 유효)  State에서 Controller로 바꿀 예정
	UPROPERTY(Replicated)
	TObjectPtr<ABNPlayerState> CurrentChallengingPlayerState;

	UPROPERTY(Replicated)
	TObjectPtr<ABNPlayerController> CurrentPlayerController;
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// 이 함수는 BeginPlay에서 OverlapBox->OnComponentEndOverlap에 바인딩됩니다.
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// TimingGaugeComponent의 OnGaugeFinished 델리게이트에 바인딩됩니다.
	UFUNCTION()
	void OnGaugeFinished(EVerticalGaugeResult Result);

#pragma region Mission1 State Variables
public:
	UPROPERTY(EditDefaultsOnly,Replicated, Category = "Mission1 | Life")
	int32 MaxMissionLife = 3;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentMissionLifeCount, VisibleAnywhere, BlueprintReadOnly, Category = "Mission1 | Life")
	int32 CurrentMissionLifeCount;

	UFUNCTION()
	void OnRep_CurrentMissionLifeCount();

	UPROPERTY(EditDefaultsOnly,Replicated, Category = "Mission1 | Success")
	int32 RequiredSuccessCount = 4;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentSuccessCount, VisibleAnywhere, BlueprintReadOnly, Category = "Mission1 | Success")
	int32 CurrentSuccessCount = 0;

	UFUNCTION()
	void OnRep_CurrentSuccessCount();

	UPROPERTY(EditAnywhere, Category = "Mission|Duration", meta=(Units="Seconds"))
	float OverallMissionDuration = 120.0f; // 미션 전체의 총 제한 시간 (예: 2분 = 120초)

	FTimerHandle OverallMissionTimerHandle;
	
	//전체시간 만료시 호출
	void HandleOverallMissionTimeOut();
#pragma endregion

#pragma region Server
public:
	// --- 미션 진행/종료 로직 (서버에서만 실행) ---
	// 클라이언트의 게이지 입력에 대한 판정을 수행합니다.
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Mission")
	void Server_PerformGaugeCheck(FGuid InGaugeID, float ClientGaugeValue);

	// 미션 시작/종료 시 호출될 서버 로직
	void StartMission(ABNPlayerController* InPlayerController);
	void EndMission(EMissionResult Result);

	// **서버 전용 로직: 미션 상태 변경 및 완료 조건 확인**
	void Server_HandleMissionSuccess();
	void Server_HandleMissionFailure();
	void Server_CheckMissionCompletion(); // 미션 완료 여부 검사 (서버 전용)
#pragma endregion

#pragma region Client
public:

	UFUNCTION(Client, Reliable)
	void Client_EndMission(EMissionResult Result);
	
#pragma endregion
};
