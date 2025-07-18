// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BaseType/BaseEnumType.h"

#include "VerticalTimingGaugeComponent.generated.h"

class UBNInGameWidget;
class ABNPlayerController;
class UImage;
class UBorder;
enum class EVerticalGaugeResult : uint8;

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
	
	// UI 활성화는 Client RPC로 직접 제어하는 것이 더 정확
	UPROPERTY() // bIsGaugeActive는 복제 불필요. 클라이언트 로컬 상태로 관리
	bool bIsGaugeActiveLocal; // 클라이언트 로컬에서만 사용될 게이지 활성화 상태 변수 (옵션)

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 해당 컴포넌트의 고유 ID (FGuid : 고유한 식별자를 의미합니다. 128비트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing Gauge | ID")
	FGuid GaugeID;

	// 게이지 위젯 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category="Timing Gauge | UI")
	TSubclassOf<UBNInGameWidget> VerticalGaugeWidgetClass;

	//현재 게이지 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UBNInGameWidget> VerticalGaugeWidgetInstance;

	//UI 요소들 바인딩
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_GaugeBackground;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Pointer;

	// 게이지 커서 움식임 속도 (0.0 ~ 1.0 사이 값, 초당) < 언제든 변경 가능
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing Gauge | Settings")
	float GaugeSpeed;
	
	// 게이지 커서의 현재 값 (0.0 ~ 1.0) - 아래쪽이 0.0, 위쪽이 1.0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timing Gauge | State")
	float CurrentGaugeValue;

	// 게이지 커서의 움직임 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TIming Gauge | State")
	float GaugeDirection;

	// 녹색(성공 영역)의 시작 지점 - 서버에서 랜덤 결정 및 복제
	UPROPERTY(ReplicatedUsing=OnRep_GreenZoneLocation, BlueprintReadOnly, Category = "Timing Gauge | Zones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GreenZoneStart;

	//녹색 영역의 길이 - 블루프린트에서 고정, 서버에서 복제
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_GreenZoneLocation, Category = "Timing Gauge | Zones", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GreenZoneLength;

	// UI활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Timing Gauge | State")
	bool bIsGaugeActive;

	//서버에 게이지 시작을 요청
	UFUNCTION(BlueprintCallable, Category="Timing Gauge | Control")
	void RequestStartGauge(const ABNPlayerController* BNPlayerController);

	//클라이언트에서 게이지 실제로 시작
	UFUNCTION(Client, Reliable)
	void Client_StartGaugeUI();

	//플레이어 입력을 처리 (클라이언트에서 서버로 결과 전송용)
	UFUNCTION(BlueprintCallable, Category = "Timing Gauge | Control")
	void HandleGaugeInput();

	// 클라이언트에서 게이지 UI를 실제 종료 (서버로 부터 호출)
	UFUNCTION(Client, Reliable)
	void Client_EndGaugeUI(EVerticalGaugeResult Result);

	/** 게이지 결과를 외부에 알리는 이벤트 (블루프린트에서 리슨 가능) */
	UPROPERTY(BlueprintAssignable, Category = "Timing Gauge | Events")
	FOnVerticalGaugeFinishedSignature OnGaugeFinished;
	
protected:
	//GreenZone위치 복제시 클라이언트에서 호출되는 함수
	UFUNCTION()
	void OnRep_GreenZoneLocation();

	//녹색 영영의 UI의 위치와 크키 업데이트
	void UpdateGreenZoneUI();

	//커서의 UI위치를 업데이트
	void UpdatePointerUI();

private:
	float CachedGaugeHeight;	//게이지 바 전체 높이 캐시
	float CachedPointerHeight;	//커서 높이 캐시

	//서버로 게이지 시작 요청 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartGaugeInternal(FGuid InGaugeID, const ABNPlayerController* BNPlayerController);

	//서버로 게이지 종료 요청 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestEndGaugeInternal(FGuid InGaugeID, EVerticalGaugeResult Result);
};
