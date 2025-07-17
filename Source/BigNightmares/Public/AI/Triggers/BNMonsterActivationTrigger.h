// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNMonsterActivationTrigger.generated.h"

class UBoxComponent;
class ABNBaseMonster;

// [추가] 에디터에서 몬스터 활성화 조건을 선택할 수 있도록 Enum을 정의합니다.
UENUM(BlueprintType)
enum class EActivationCondition : uint8
{
	// 플레이어가 볼륨에 진입했을 때 활성화됩니다. (기존 방식)
	OnPlayerOverlap,
	
	// 특정 시간이 경과했을 때 활성화됩니다.
	OnTimeElapsed,

	// 특정 미션 갯수를 클리어했을 때 활성화됩니다.
	OnMissionCount,

	// 다른 블루프린트나 C++ 코드에서 직접 호출할 때만 활성화됩니다.
	OnExternalEvent
};

UCLASS()
class BIGNIGHTMARES_API ABNMonsterActivationTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ABNMonsterActivationTrigger();
	
	// [추가] 외부(게임 스테이트, 레벨 블루프린트 등)에서 이 트리거를 직접 발동시킬 때 호출하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void TryActivate();

protected:
	virtual void BeginPlay() override;

	// 트리거의 범위를 나타내는 박스 컴포넌트입니다. OnPlayerOverlap 조건일 때만 사용됩니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	// --- 활성화 조건 설정 ---
	// 에디터에서 몬스터를 활성화할 조건을 선택합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	EActivationCondition ActivationCondition = EActivationCondition::OnPlayerOverlap;

	// [추가] OnTimeElapsed 조건일 때, 활성화될 때까지 기다릴 시간(초)입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnTimeElapsed"))
	float TimeToWait = 5.0f;

	// [추가] OnMissionCount 조건일 때, 활성화에 필요한 미션 클리어 갯수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (EditCondition = "ActivationCondition == EActivationCondition::OnMissionCount"))
	int32 RequiredMissionCount = 0;
	// ---

	// 에디터에서 활성화시킬 몬스터들을 지정할 수 있는 배열입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	TArray<TObjectPtr<ABNBaseMonster>> MonstersToActivate;

	// 트리거가 한 번만 작동하도록 설정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings")
	bool bTriggerOnce = true;

private:
	// 다른 액터가 이 볼륨에 오버랩(진입)했을 때 호출될 함수입니다.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 실제로 몬스터들을 활성화시키는 핵심 로직 함수입니다.
	void ActivateMonsters();

	// 타이머가 만료되었을 때 호출될 함수입니다.
	void OnTimerFinished();

	// 트리거가 이미 작동했는지 여부를 저장하는 변수입니다.
	bool bHasBeenTriggered = false;

};
