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

	// ✅ 외부에서 몬스터를 등록할 수 있도록 하는 함수 추가
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void AddMonsterToActivate(class ABNBaseMonster* Monster);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	EActivationCondition ActivationCondition = EActivationCondition::OnPlayerOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnTimeElapsed"))
	float TimeToWait = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnMissionCount"))
	int32 RequiredMissionCount = 0;

	// ✅ protected 그대로 유지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	TArray<TObjectPtr<ABNBaseMonster>> MonstersToActivate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	bool bTriggerOnce = true;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActivateMonsters();
	void OnTimerFinished();

	bool bHasBeenTriggered = false;
};
