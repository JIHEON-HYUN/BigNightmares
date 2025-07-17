// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Mission/BaseMissionActor.h"
#include "MissionTimingGauge.generated.h"

enum class EVerticalGaugeResult : uint8;
class ABNPlayerState;
class UVerticalTimingGaugeComponent;

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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing Gauge")
	TObjectPtr<UVerticalTimingGaugeComponent> TimingGaugeComponent;
	
protected:
	//현재 이 미니게임에 도전 중인 플레이어 (서버에서만 유효)
	UPROPERTY()
	TObjectPtr<ABNPlayerState> CurrentChallengingPlayerState;
	
	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// 이 함수는 BeginPlay에서 OverlapBox->OnComponentEndOverlap에 바인딩됩니다.
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// TimingGaugeComponent의 OnGaugeFinished 델리게이트에 바인딩됩니다.
	UFUNCTION()
	void OnGaugeFinished(EVerticalGaugeResult Result);

};
