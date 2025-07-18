// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNMonsterActivationTrigger.generated.h"

class UBoxComponent;
class ABNBaseMonster;

// 몬스터 활성화 조건 Enum
UENUM(BlueprintType)
enum class EActivationCondition : uint8
{
	// 플레이어 진입
	OnPlayerOverlap,
	
	// 시간 경과 시
	OnTimeElapsed,

	// 미션 카운트 달성 시
	OnMissionCount,

	// 외부 이벤트 호출 시
	OnExternalEvent
};

UCLASS()
class BIGNIGHTMARES_API ABNMonsterActivationTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	// 생성자
	ABNMonsterActivationTrigger();
	
	// 외부에서 트리거를 직접 발동시키는 함수
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void TryActivate();

protected:
	// 액터 생성 시 최초 실행 함수
	virtual void BeginPlay() override;

	// 트리거 범위 Box 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	// --- 활성화 조건 설정 ---
	// 활성화 조건 선택 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	EActivationCondition ActivationCondition = EActivationCondition::OnPlayerOverlap;

	// OnTimeElapsed 조건 시 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnTimeElapsed"))
	float TimeToWait = 5.0f;

	// OnMissionCount 조건 시 필요 미션 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnMissionCount"))
	int32 RequiredMissionCount = 0;
	// ---

	// 활성화시킬 몬스터 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	TArray<TObjectPtr<ABNBaseMonster>> MonstersToActivate;

	// 한 번만 발동할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	bool bTriggerOnce = true;

private:
	// 오버랩(진입) 이벤트 발생 시 호출될 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 몬스터 활성화 로직 함수
	void ActivateMonsters();

	// 타이머 만료 시 호출될 함수
	void OnTimerFinished();

	// 트리거 발동 여부 저장 변수
	bool bHasBeenTriggered = false;
};
