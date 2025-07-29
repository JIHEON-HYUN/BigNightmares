// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BNMission1Widget.generated.h"

class ABNPlayerController;
class UTextBlock;
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

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	//스페이스바가 눌렸을 때의 로직을 담당
	UFUNCTION()
	void HandleSpaceBarPressed();

	//ESC키를 눌렀을 떄의 로직을 담당
	UFUNCTION()
	void HandleEscapePressed();
	
public:
	//녹색 영영의 UI의 위치와 크키 업데이트
	void UpdateGreenZoneUI(float GreenZoneStart, float GreenZoneLength);

	//커서의 UI위치를 업데이트
	void UpdatePointerUI(float CurrentGaugeValue);

#pragma region UI Components
	//UI 요소들 바인딩
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_GaugeBackground;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Pointer;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> LifeCountText; 

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> SuccessCountText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MaxLifeText; // 최대 라이프를 표시할 텍스트 (예: "3 / 3")

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RequiredSuccessText; // 필요 성공 횟수를 표시할 텍스트 (예: "0 / 5")

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Life1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Life2;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Life3;

	UPROPERTY(Transient) // 런타임에 채워지는 배열이므로 Transient
	TArray<UImage*> LifeIcons;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Success1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Success2;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Success3;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Success4;

	UPROPERTY(Transient) // 런타임에 채워지는 배열이므로 Transient
	TArray<UImage*> SuccessIcons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Textures")
	TObjectPtr<UTexture2D> Life_Active_Tex; // 활성화된 라이프 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Textures")
	TObjectPtr<UTexture2D> Life_Inactive_Tex; // 비활성화된 라이프 이미지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Textures")
	TObjectPtr<UTexture2D> Success_Active_Tex; // 활성화된 성공 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Textures")
	TObjectPtr<UTexture2D> Success_Inactive_Tex; // 비활성화된 성공 이미지
	
#pragma endregion

#pragma region Cached UI Metrics
	// 캐시된 게이지 배경의 Canvas Panel 상 Y 위치
	UPROPERTY()
	float CachedGaugeBackgroundPosY = 0.f;

	UPROPERTY()
	float CachedGaugeBackgroundHeight;	//게이지 바 전체 높이 캐시

	UPROPERTY()
	float CachedPointerHeight;	//커서 높이 캐시
	
#pragma endregion

#pragma region Logic Value
	UPROPERTY()
	float SmoothedGaugeValue;

	/**
	 * 위젯이 데이터를 가져올 UVerticalTimingGaugeComponent의 약한 참조
	 * 컴포넌트가 파괴되더라도 위젯에 문제가 발생하지 않도록 TWeakObjectPtr를 사용
	 */
	TWeakObjectPtr<UVerticalTimingGaugeComponent> OwningGaugeComponent;

	// 현재 미션의 최대 라이프와 필요 성공 횟수를 저장하는 변수
	int32 CachedMaxLife;
	int32 CachedRequiredSuccess;

private:
	UPROPERTY()
	TObjectPtr<ABNPlayerController> BNPC;

#pragma endregion

public:
	//미션 목표 수와 초기 라이프 수를 설정하여 UI초기화 함수
	UFUNCTION(BlueprintCallable, Category="Mission UI")
	void SetMissionGoals(int32 InMaxLife, int32 InRequiredSuccess);

	// 미션 라이프 카운트를 UI에 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "Mission UI")
	void UpdateLifeUI(int32 NewLifeCount);

	// 미션 성공 카운트를 UI에 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "Mission UI")
	void UpdateSuccessUI(int32 NewSuccessCount);

	void SetIconState(UImage* TargetImage, bool bIsActive, UTexture2D* ActiveTexture, UTexture2D* InactiveTexture);
};
