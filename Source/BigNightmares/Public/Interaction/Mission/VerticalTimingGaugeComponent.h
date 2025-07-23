// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BaseType/BaseEnumType.h"

#include "VerticalTimingGaugeComponent.generated.h"

class UBNInGameWidget;
class ABNPlayerController;

// 결과 전달을 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVerticalGaugeFinishedSignature, EVerticalGaugeResult, Result);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIGNIGHTMARES_API UVerticalTimingGaugeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVerticalTimingGaugeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 복제 속성 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 해당 컴포넌트의 고유 ID (FGuid : 고유한 식별자를 의미합니다. 128비트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing Gauge | ID")
	FGuid GaugeID;
	
	// 게이지 커서 움식임 속도 (0.0 ~ 1.0 사이 값, 초당) < 언제든 변경 가능
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing Gauge | Settings")
	float GaugeSpeed;
	
	// 게이지 커서의 현재 값 (0.0 ~ 1.0) - 아래쪽이 0.0, 위쪽이 1.0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Timing Gauge | State")
	float CurrentGaugeValue;

	// 게이지 커서의 움직임 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "TIming Gauge | State")
	float GaugeDirection;

	// 녹색(성공 영역)의 시작 지점 - 서버에서 랜덤 결정 및 복제
	UPROPERTY(ReplicatedUsing=OnRep_GreenZoneLocation, BlueprintReadOnly, Category = "Timing Gauge | Zones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GreenZoneStart;

	//녹색 영역의 길이 - 블루프린트에서 고정, 서버에서 복제
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_GreenZoneLocation, Category = "Timing Gauge | Zones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GreenZoneLength;
	
	//서버에 게이지 시작을 요청
	UFUNCTION(BlueprintCallable, Category="Timing Gauge | Control")
	void RequestStartGauge(ABNPlayerController* BNPlayerController);

	//플레이어 입력을 처리 (클라이언트에서 서버로 결과 전송용) //TODO : Controller로 위치 변경 할 수도?
	UFUNCTION(BlueprintCallable, Category = "Timing Gauge | Control")
	void HandleGaugeInput();

	/** 게이지 결과를 외부에 알리는 이벤트 (블루프린트에서 리슨 가능) */
	UPROPERTY(BlueprintAssignable, Category = "Timing Gauge | Events")
	FOnVerticalGaugeFinishedSignature OnGaugeFinished;

	// 게이지 컴포넌트의 게임 로직이 활성화되어 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Timing Gauge | State")
	bool bIsLogicActive;
	
protected:
	//GreenZone위치 복제시 클라이언트에서 호출되는 함수
	UFUNCTION()
	void OnRep_GreenZoneLocation();
private:
	
	//서버로 게이지 시작 요청 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartGaugeInternal(FGuid InGaugeID, ABNPlayerController* BNPlayerController);

	//서버로 게이지 종료 요청 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestEndGaugeInternal(FGuid InGaugeID, EVerticalGaugeResult Result);
};
