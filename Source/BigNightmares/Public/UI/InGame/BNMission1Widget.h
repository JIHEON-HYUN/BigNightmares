// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNMission1Widget.generated.h"

class UVerticalTimingGaugeComponent;
class UImage;
class UBorder;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API UBNMission1Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="BN Mission 1 Widget")
	void SetGaugeComponent(UVerticalTimingGaugeComponent* InComponent);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	//UI 요소들 바인딩
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_GaugeBackground;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Pointer;

	// 캐시된 게이지 배경의 Canvas Panel 상 Y 위치
	UPROPERTY()
	float CachedGaugeBackgroundPosY = 0.f;

	UPROPERTY()
	float SmoothedGaugeValue;

	/**
	 * 위젯이 데이터를 가져올 UVerticalTimingGaugeComponent의 약한 참조
	 * 컴포넌트가 파괴되더라도 위젯에 문제가 발생하지 않도록 TWeakObjectPtr를 사용
	 */
	TWeakObjectPtr<UVerticalTimingGaugeComponent> OwningGaugeComponent;

	//녹색 영영의 UI의 위치와 크키 업데이트
	void UpdateGreenZoneUI(float GreenZoneStart, float GreenZoneLength);

	//커서의 UI위치를 업데이트
	void UpdatePointerUI(float CurrentGaugeValue);

	float CachedGaugeBackgroundHeight;	//게이지 바 전체 높이 캐시
	float CachedPointerHeight;	//커서 높이 캐시
};
