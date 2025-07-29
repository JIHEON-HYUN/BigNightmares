// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/BNMission1Widget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController/BNPlayerController.h"

#include "Interaction/Mission/VerticalTimingGaugeComponent.h"

void UBNMission1Widget::SetGaugeComponent(UVerticalTimingGaugeComponent* InComponent)
{
	OwningGaugeComponent = InComponent;
	if (IsValid(OwningGaugeComponent.Get()))
	{
		UpdateGreenZoneUI(OwningGaugeComponent->GreenZoneStart, OwningGaugeComponent->GreenZoneLength);
		UpdatePointerUI(OwningGaugeComponent->CurrentGaugeValue);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UBNMission1Widget::SetGaugeComponent : Gauge component is null"));
	}
}

void UBNMission1Widget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("BNMission1Widget::NativeConstruct called. Checking BindWidget properties."));
	
	// UPROPERTY(meta = (BindWidget))을 사용했으므로 여기서 GetWidgetFromName은 필요없음
	// 변수들이 올바르게 바인딩되었는지 유효성 검사만 수행.
	if (!IsValid(Border_GaugeBackground) || !IsValid(Image_Pointer) || !IsValid(Image_Green))
	{
		RemoveFromParent();
		//필수 위젯 소가 없으면 위젯을 화면에서 제거하여 UI표시 없음.
		return;
	}

	if (CachedGaugeBackgroundHeight <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BNMission1Widget::NativeConstruct : Gauge background height is zero or negative (%.2f). UI may not display correctly."), CachedGaugeBackgroundHeight);
	}
	if (CachedPointerHeight <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("BNMission1Widget::NativeConstruct : Pointer height is zero or negative (%.2f). UI may not display correctly."), CachedPointerHeight);
	}

	BNPC = Cast<ABNPlayerController>(GetOwningPlayer());
	if (!IsValid(BNPC))
	{
		RemoveFromParent();
		return;
	}

	//위젯 추가
	if (Image_Life1) LifeIcons.Add(Image_Life1);
	if (Image_Life2) LifeIcons.Add(Image_Life2);
	if (Image_Life3) LifeIcons.Add(Image_Life3);

	if (Image_Success1) SuccessIcons.Add(Image_Success1);
	if (Image_Success2) SuccessIcons.Add(Image_Success2);
	if (Image_Success3) SuccessIcons.Add(Image_Success3);
	if (Image_Success4) SuccessIcons.Add(Image_Success4);

	if (LifeIcons.Num() == 0 || SuccessIcons.Num() == 0)
	{
		RemoveFromParent();
		return;
	}
}

void UBNMission1Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(Border_GaugeBackground))
	{
		float CurrentRenderedHeight = Border_GaugeBackground->GetCachedGeometry().GetLocalSize().Y;
		// Cached 값이 0이거나 현재 렌더링 크기와 다를 때 업데이트
		if (!FMath::IsNearlyEqual(CachedGaugeBackgroundHeight, CurrentRenderedHeight, KINDA_SMALL_NUMBER) || CachedGaugeBackgroundHeight <= 0.f)
		{
			UCanvasPanelSlot* BackgroundSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_GaugeBackground);
			CachedGaugeBackgroundHeight = CurrentRenderedHeight;
			if (BackgroundSlot)
			{
				// 게이지 배경의 실제 Canvas Panel 상 Y 위치를 캐시합니다.
				CachedGaugeBackgroundPosY = BackgroundSlot->GetPosition().Y;
				//UE_LOG(LogTemp, Log, TEXT("NativeTick - CachedGaugeBackgroundPosY: %.2f"), CachedGaugeBackgroundPosY);
			}
			
			// 높이가 0이 아닌 유효한 값으로 업데이트되면 녹색 영역 UI를 다시 초기화합니다.
			if (CachedGaugeBackgroundHeight > 0.f && OwningGaugeComponent.IsValid())
			{
				UpdateGreenZoneUI(OwningGaugeComponent->GreenZoneStart, OwningGaugeComponent->GreenZoneLength);
			}
		}
	}

	if (IsValid(Image_Pointer)) // Image_Pointer가 유효하다는 것을 확인했으므로, 이 유효성 검사는 필요합니다.
	{
		float CurrentPointerHeight = Image_Pointer->GetCachedGeometry().GetLocalSize().Y;
		// Cached 값이 0이거나 현재 렌더링 크기와 다를 때 업데이트
		if (!FMath::IsNearlyEqual(CachedPointerHeight, CurrentPointerHeight, KINDA_SMALL_NUMBER) || CachedPointerHeight <= 0.f)
		{
			CachedPointerHeight = CurrentPointerHeight;
			// 포인터 높이가 업데이트되면 다음 UpdatePointerUI 호출 시 반영될 것입니다.
		}
	}

	if (OwningGaugeComponent.IsValid())
	{
		if (OwningGaugeComponent->bIsLogicActive)
		{
			float TargetGaugeValue = OwningGaugeComponent->CurrentGaugeValue;
			SmoothedGaugeValue = FMath::FInterpTo(SmoothedGaugeValue, TargetGaugeValue, InDeltaTime, 15.f);
			
			UpdatePointerUI(OwningGaugeComponent->CurrentGaugeValue);
		}
		else // 게이지가 비활성화된 상태
		{
			// SmoothedGaugeValue를 0.0f로 서서히 이동시켜 포인터를 맨 아래로 되돌림
			if (!FMath::IsNearlyEqual(SmoothedGaugeValue, 0.0f, KINDA_SMALL_NUMBER))
			{
				SmoothedGaugeValue = FMath::FInterpTo(SmoothedGaugeValue, 0.0f, InDeltaTime, 5.f);
				UpdatePointerUI(SmoothedGaugeValue);
			}
			else // 0.0f에 도달하면 더 이상 업데이트하지 않음 (성능 최적화)
			{
				// 필요하다면 여기에 UI 숨김 로직 등을 추가
			}
		}
	}
}

FReply UBNMission1Widget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		HandleSpaceBarPressed();
		
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		HandleEscapePressed();
		
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBNMission1Widget::HandleSpaceBarPressed()
{
	//UE_LOG(LogTemp, Log, TEXT("BNMission1Widget::HandleSpaceBarPressed called.")); // 이 로그가 호출되는지 확인

	if (OwningGaugeComponent.IsValid())
	{
		//UE_LOG(LogTemp, Log, TEXT("BNMission1Widget::HandleSpaceBarPressed: OwningGaugeComponent is valid. Attempting to call Server_ProcessGaugeInput()."));
		if (IsValid(BNPC))
		{
			BNPC->Server_ReportGaugeInput(OwningGaugeComponent->GaugeID, SmoothedGaugeValue);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UBNMission1Widget::HandleSpaceBarPressed : Not IsValid BNPC"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BNMission1Widget::HandleSpaceBarPressed: OwningGaugeComponent is INVALID. Cannot process input.")); // 이 로그가 찍힌다면 문제
	}
}

void UBNMission1Widget::HandleEscapePressed()
{
	
}

void UBNMission1Widget::UpdateGreenZoneUI(float GreenZoneStart, float GreenZoneLength)
{
	if (!IsValid(Image_Green) || CachedGaugeBackgroundHeight <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("BNMission1Widget::UpdateGreenZoneUI: Invalid Image_Green or CachedGaugeBackgroundHeight (%.2f)."), CachedGaugeBackgroundHeight);
		return;
	}

	float GreenZoneHeight = CachedGaugeBackgroundHeight * GreenZoneLength;
	
	float GreenZonePosY = CachedGaugeBackgroundHeight * (1.0f -  (GreenZoneStart + GreenZoneLength));

	float GreenZoneFinalPosY = CachedGaugeBackgroundPosY + GreenZonePosY;

	// Image_Green 위젯의 슬롯을 CanvasPanelSlot으로 캐스팅하여 위치와 크기를 변경합니다.
	// SlotAsCanvasSlot : 특정 위젯이 UCanvasPanel에 배치되어 있는지 확인 후, 그 위젯의 슬롯에 대해 UCanvasPanel만의 고유한 위치, 크기, 정렬 등의 레이아웃 속성을 설정하고 싶을 때 사용
	UCanvasPanelSlot* GreenSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Green);
	if (GreenSlot)
	{
		// Y위치만 업데이트
		GreenSlot->SetPosition(FVector2D(GreenSlot->GetPosition().X, GreenZoneFinalPosY));
		GreenSlot->SetSize(FVector2D(GreenSlot->GetSize().X, GreenZoneHeight));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BNMission1Widget::UpdateGreenZoneUI: Failed to cast Image_Green to UCanvasPanelSlot."));
	}
}

void UBNMission1Widget::UpdatePointerUI(float CurrentGaugeValue)
{
	if (!IsValid(Image_Pointer) || CachedGaugeBackgroundHeight <= 0.f || CachedPointerHeight <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("BNMission1Widget::UpdatePointerUI: Invalid Image_Pointer or cached heights (Background: %.2f, Pointer: %.2f)."),
			CachedGaugeBackgroundHeight, CachedPointerHeight);
		return;
	}
	
	UCanvasPanelSlot* PointerSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Pointer);

	float MinPointerY = CachedGaugeBackgroundPosY;
	float MaxPointerY = CachedGaugeBackgroundPosY + CachedGaugeBackgroundHeight - CachedPointerHeight; 

	float FinalPointerPosY = FMath::Lerp(MaxPointerY, MinPointerY, CurrentGaugeValue);

	if (PointerSlot)
	{
		PointerSlot->SetPosition(FVector2D(PointerSlot->GetPosition().X, FinalPointerPosY));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BNMission1Widget::UpdatePointerUI: Failed to cast Image_Pointer to UCanvasPanelSlot."));
	}
}

void UBNMission1Widget::SetMissionGoals(int32 InMaxLife, int32 InRequiredSuccess)
{
	CachedMaxLife = InMaxLife;
	CachedRequiredSuccess = InRequiredSuccess;

	if (MaxLifeText)
	{
		MaxLifeText->SetText(FText::AsNumber(InMaxLife));
	}
	if (RequiredSuccessText)
	{
		RequiredSuccessText->SetText(FText::AsNumber(InRequiredSuccess));
	}

	//초기 라이프와 성공횟수 업데이트
	UpdateLifeUI(InMaxLife);
	UpdateSuccessUI(0);
}

void UBNMission1Widget::UpdateLifeUI(int32 NewLifeCount)
{
 	if (LifeCountText)
	{
		FText FormattedText = FText::Format(
			NSLOCTEXT("MissionUI", "LifeCountFormat", "라이프: {0} / {1}"),
			FText::AsNumber(NewLifeCount),
			FText::AsNumber(CachedMaxLife)
		);
		LifeCountText->SetText(FormattedText);
	}

	for (int32 i = 0; i < LifeIcons.Num(); ++i)
	{
		if (IsValid(LifeIcons[i]))
		{
			// 현재 라이프 수보다 인덱스가 작거나 같으면 활성화 (0부터 시작하므로 +1)
			SetIconState(LifeIcons[i], (i + 1) <= NewLifeCount, Life_Active_Tex, Life_Inactive_Tex);
		}
	}
}

void UBNMission1Widget::UpdateSuccessUI(int32 NewSuccessCount)
{
  	if (SuccessCountText)
	{
		FText FormattedText = FText::Format(
			NSLOCTEXT("MissionUI", "SuccessCountFormat", "성공: {0} / {1}"),
			FText::AsNumber(NewSuccessCount),
			FText::AsNumber(CachedRequiredSuccess) // SetMissionGoals에서 설정된 필요 성공 횟수 사용
		);
		SuccessCountText->SetText(FormattedText);
	}

	for (int32 i = 0; i < SuccessIcons.Num(); ++i)
	{
		if (IsValid(SuccessIcons[i]))
		{
			// 현재 성공 수보다 인덱스가 작거나 같으면 활성화 (0부터 시작하므로 +1)
			SetIconState(SuccessIcons[i], (i + 1) <= NewSuccessCount, Success_Active_Tex, Success_Inactive_Tex);
		}
	}
}

void UBNMission1Widget::SetIconState(UImage* TargetImage, bool bIsActive, UTexture2D* ActiveTexture,
	UTexture2D* InactiveTexture)
{
	if (!IsValid(TargetImage))
	{
		return;
	}

	FSlateBrush NewBrush = TargetImage->GetBrush(); // 현재 브러시 복사

	// 활성화 여부에 따라 적절한 텍스처 할당
	NewBrush.SetResourceObject(bIsActive ? ActiveTexture : InactiveTexture);
	
	//NewBrush.ImageSize = FVector2D(TargetImage->GetDesiredSize()); // 혹은 고정 크기 FVector2D(500.f, 500.f);
	NewBrush.ImageSize = FVector2D(150.f, 150.f); // 혹은 고정 크기 FVector2D(500.f, 500.f);

	TargetImage->SetBrush(NewBrush);
}
