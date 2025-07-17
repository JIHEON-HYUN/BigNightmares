// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/VerticalTimingGaugeComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UVerticalTimingGaugeComponent::UVerticalTimingGaugeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	//bAlwaysRelevant = true; //멀리 있어도 항상 복제 되도록 설정 (AActor 자료형에서 가능)

	GaugeSpeed = 0.5f; //커서 움직임 속도 (초당 0.5f로 이동) / 한번 성공할 때 마다 값이 커지게
	CurrentGaugeValue  = 0.0f; //0아래, 1위
	GaugeDirection = 1.0f; //초기에 아래에서 위로 움직이면서 시작,

	GreenZoneStart = 0.0f; //0.0은 기본값, 적용될 값은 서버에서 랜덤으로 결정할 것
	GreenZoneLength = 0.1f; //블루프린트에서 설정될 값

	bIsGaugeActive = false;
	CachedGaugeHeight = 0.0f;
	CachedPointerHeight = 0.0f;
}


// Called when the game starts
void UVerticalTimingGaugeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVerticalTimingGaugeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//녹색 영역의 시작 위치와 길이를 모든 클라에 복제
	DOREPLIFETIME(UVerticalTimingGaugeComponent, GreenZoneStart);
	DOREPLIFETIME(UVerticalTimingGaugeComponent, GreenZoneLength);
}

void UVerticalTimingGaugeComponent::RequestStartGauge()
{
	if (!GetWorld()) return;

	if (!GaugeID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GaugeID is not set for this UVerticalTimingGaugeComponent. Cannot request start."));
		return;
	}

	if (bIsGaugeActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gauge ID '%s' is already active. Ignoring start request."), *GaugeID.ToString());
		return;
	}

	// 클라이언트든 서버든 (리스닝 서버의 호스트 클라이언트 부분 포함),
	// 게이지 시작 요청은 항상 서버 RPC를 통해 이루어집니다.
	Server_RequestStartGaugeInternal(GaugeID);
}

// Server_RequestStartGaugeInternal RPC 유효성 검사
bool UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Validate(FGuid InGaugeID)
{
	//1. 게이지 ID의 유효성 검사
	//GameState의 유효성
	ABNGameState* GS = GetWorld()->GetGameState<ABNGameState>();
	if (!IsValid(GS))
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Invalid GameState. Rejecting RPC for GaugeID: %s"), *InGaugeID.ToString());
		return false;
	}

	//서버가 해당 GaugeID에 대한 정보를 가지고 있는지 확인
	if (this->GaugeID !=  InGaugeID)  // 이 컴포넌트의 실제 GaugeID와 RPC로 받은 InGaugeID가 일치하는지
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Mismatched GaugeID. RPC GaugeID: %s, Component ID: %s"), *InGaugeID.ToString(), *this->GaugeID.ToString());
		return false;
	}
	
	return true;
}

// Server_RequestStartGaugeInternal RPC 구현 (서버에서 실행)
void UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Implementation(FGuid InGaugeID)
{
	ABNPlayerController* BNPlayerController = Cast<ABNPlayerController>(GetOwner()->GetInstigatorController());
	if (!BNPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_RequestStartGaugeInternal: Requesting BNPlayerController is null."));
		return;
	}

	ABNGameState* GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS && GS->Server_TryStartSpecificGaugeChallenge(GaugeID, BNPlayerController))
	{
		//서버에서 GreenZone의 위치를 랜덤 결정
		GreenZoneStart = FMath::FRandRange(0.0f, 1.0f - GreenZoneLength);
		//해당 변수는 값이 바뀌는 순간 ReplicatedUsing에 의해 클라에 복제

		Client_StartGaugeUI(); //서버 자신의 클라이언트 부분도 UI시작	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Client's request to start gauge challenge ID '%s' denied by GameState."), *InGaugeID.ToString());
	}
}

void UVerticalTimingGaugeComponent::Client_StartGaugeUI_Implementation()
{
	//클라 실행
	//서버로부터 '게이지 UI를 시작하라'는 명령을 받았을 때만 실행
	if (!VerticalGaugeWidgetClass || !GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("VerticalGaugeWidgetClass not set or World is null for ID %s!"), *GaugeID.ToString());
		return;
	}

	//기존 위젯이 있다면 제거 (시작시)
	if (VerticalGaugeWidgetInstance)
	{
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;

		ABNPlayerController* BNPlayerController = Cast<ABNPlayerController>(GetWorld()->GetFirstPlayerController());
		if (BNPlayerController)
		{
			VerticalGaugeWidgetInstance = CreateWidget<UUserWidget>(BNPlayerController, VerticalGaugeWidgetClass);
			if (VerticalGaugeWidgetInstance)
			{
				VerticalGaugeWidgetInstance->AddToViewport();
				if (!Border_GaugeBackground || !Image_Green || !Image_Pointer)
				{
					Server_RequestEndGaugeInternal(GaugeID, EVerticalGaugeResult::EVGR_Fail);
					return;
				}

			}
			
			CachedGaugeHeight = Border_GaugeBackground->GetCachedGeometry().GetLocalSize().Y;
			if (CachedGaugeHeight == 0.0f)
			{
				CachedPointerHeight = 400.0f;
				UE_LOG(LogTemp, Warning, TEXT("CachedGaugeHeight was 0.0f for ID %s. Defaulting to %f."), *GaugeID.ToString(), CachedGaugeHeight);
			}
			CachedPointerHeight =Image_Pointer->GetCachedGeometry().GetLocalSize().Y;
			if (CachedPointerHeight == 0.0f)
			{
				CachedPointerHeight = 50.0f;
				UE_LOG(LogTemp, Warning, TEXT("CachedPointerHeight was 0.0f for ID %s. Defaulting to %f."), *GaugeID.ToString(), CachedPointerHeight);
			}

			UpdateGreenZoneUI();
			CurrentGaugeValue = 0.0f;
			GaugeDirection = 1.0f;
			bIsGaugeActive = true;
			UE_LOG(LogTemp, Log, TEXT("Client: Gauge UI for ID '%s' started."), *GaugeID.ToString());
		}
	}
}

void UVerticalTimingGaugeComponent::HandleGaugeInput()
{
	if (!bIsGaugeActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gauge ID '%s': Input received but gauge is not active."), *GaugeID.ToString());
		return;
	}

	EVerticalGaugeResult Result = EVerticalGaugeResult::EVGR_Fail;

	if (CurrentGaugeValue >= GreenZoneStart && CurrentGaugeValue <= (GreenZoneStart + GreenZoneLength))
	{
		Result = EVerticalGaugeResult::EVGR_Success;
	}
	else
	{
		Result = EVerticalGaugeResult::EVGR_Fail;
	}

	if (GetOwnerRole() == ROLE_Authority) //클라이언트라면
	{
		Server_RequestEndGaugeInternal(GaugeID, Result);
	}
	else //서버라면 (둘이 같은 구분인데 조건때문에 일부러 분기)
	{
		Server_RequestEndGaugeInternal(GaugeID, Result);
	}
	
}

void UVerticalTimingGaugeComponent::Server_RequestEndGaugeInternal_Implementation(FGuid InGaugeID,
	EVerticalGaugeResult Result)
{
	ABNPlayerController* BNPlayerController = Cast<ABNPlayerController>(GetOwner()->GetInstigatorController());
	if (!BNPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Server_RequestEndGaugeInternal: Requesting PlayerController is null."));
		return;
	}

	ABNGameState* GS = GetWorld()->GetGameState<ABNGameState>();
	if (GS)
	{
		GS->Server_EndSpecificGaugeChallenge(InGaugeID, BNPlayerController);
	}

	Client_EndGaugeUI(Result);
}

bool UVerticalTimingGaugeComponent::Server_RequestEndGaugeInternal_Validate(FGuid InGaugeID,
	EVerticalGaugeResult Result)
{
	//1. 게이지 ID의 유효성 검사
	//GameState의 유효성
	ABNGameState* GS = GetWorld()->GetGameState<ABNGameState>();
	if (!IsValid(GS))
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Invalid GameState. Rejecting RPC for GaugeID: %s"), *InGaugeID.ToString());
		return false;
	}

	//서버가 해당 GaugeID에 대한 정보를 가지고 있는지 확인
	if (this->GaugeID !=  InGaugeID)  // 이 컴포넌트의 실제 GaugeID와 RPC로 받은 InGaugeID가 일치하는지
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Mismatched GaugeID. RPC GaugeID: %s, Component ID: %s"), *InGaugeID.ToString(), *this->GaugeID.ToString());
		return false;
	}
	
	return true;
}

void UVerticalTimingGaugeComponent::Client_EndGaugeUI_Implementation(EVerticalGaugeResult Result)
{
	//클라 실행
	//서버로부터 UI종료 명령을 받았을 때 실횅
	bIsGaugeActive = false;
	if (VerticalGaugeWidgetInstance)
	{
		VerticalGaugeWidgetInstance->RemoveFromParent();
		VerticalGaugeWidgetInstance = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Client: Gauge UI for ID '%s' ended."), *GaugeID.ToString());
	}
	
	OnGaugeFinished.Broadcast(Result);
}

void UVerticalTimingGaugeComponent::OnRep_GreenZoneLocation()
{
	if (bIsGaugeActive && IsValid(VerticalGaugeWidgetInstance))
	{
		UpdatePointerUI();
		UE_LOG(LogTemp, Log, TEXT("Client: GreenZoneLocation replicated for ID '%s'. Start: %f, Length: %f"), 
			*GaugeID.ToString(), GreenZoneStart, GreenZoneLength);
	}
}

void UVerticalTimingGaugeComponent::UpdateGreenZoneUI()
{
	if (Image_Green && Border_GaugeBackground)
	{
		//UMG UI에서 위젯의 위치나 크기를 런타임에 동적으로 변경하고 싶을 때, 부모 패널의 슬롯 설정을 변경하는 것이 일반적
		//Image_Green->Slot 특정 위젯이 부모 패널에 의해 할당받은 '슬롯' 객체를 가져오는 멤버
		UCanvasPanelSlot* GreenZoneSlot = Cast<UCanvasPanelSlot>(Image_Green->Slot);
		UCanvasPanelSlot* GaugeBackGroundSlot = Cast<UCanvasPanelSlot>(Border_GaugeBackground->Slot);

		if (GreenZoneSlot && GaugeBackGroundSlot)
		{
			// GetCachedGeometry() : 위젯의 현재 위치(절대 좌표), 크기(로컬 및 절대 크기), 스케일, 변환(transform) 등 레이아웃에 관련된 모든 정보를 포함
			FVector2D BackgroundSize = Border_GaugeBackground->GetCachedGeometry().GetLocalSize();
			if (BackgroundSize.IsZero())
			{
				// WBP_VerticalTimingGauge의 Border_GaugeBackground 크기와 일치해야 함
				BackgroundSize = FVector2D(50.0f, 800.f);
			}

			float GaugeWidth = BackgroundSize.X;
			float GaugeHeight = BackgroundSize.Y;
			FVector2D GaugeBackgroundPos = GaugeBackGroundSlot->GetPosition();

			float ZonePixelHeight = GaugeHeight * GreenZoneLength;

			float ZonePixelY = GaugeBackgroundPos.Y + (GaugeHeight - (GreenZoneStart + GreenZoneLength) * GaugeHeight);

			GreenZoneSlot->SetPosition(FVector2D(GaugeBackgroundPos.X, ZonePixelY));
			GreenZoneSlot->SetSize(FVector2D(GaugeWidth, ZonePixelHeight));
		}
	}
}

void UVerticalTimingGaugeComponent::UpdatePointerUI()
{
	if (Image_Pointer && Border_GaugeBackground)
	{
		UCanvasPanelSlot* PointerSlot = Cast<UCanvasPanelSlot>(Image_Pointer->Slot);
		UCanvasPanelSlot* GaugeBackgroundSlot = Cast<UCanvasPanelSlot>(Border_GaugeBackground->Slot);

		if (PointerSlot && GaugeBackgroundSlot)
		{
			FVector2D BackgroundSize = Border_GaugeBackground->GetCachedGeometry().GetLocalSize();
			if (BackgroundSize.IsZero())
			{
				BackgroundSize = FVector2D(50.0f, 800.f);
			}

			float GaugeWidth = BackgroundSize.X;
			float GaugeHeight = BackgroundSize.Y;
			float PointerWidth = Image_Pointer->GetCachedGeometry().GetLocalSize().X;
			float PointerHeight = Image_Pointer->GetCachedGeometry().GetLocalSize().Y;
			
			FVector2D GaugeBackgroundPos = GaugeBackgroundSlot->GetPosition();

			// 커서의 Y 위치 계산 (CurrentGaugeValue 0.0~1.0을 픽셀 위치로 변환)
			// CurrentGaugeValue가 0.0 (게이지 맨 아래)일 때, Y는 GaugeHeight - PointerHeight (UMG 기준)
			// CurrentGaugeValue가 1.0 (게이지 맨 위)일 때, Y는 0.0 (UMG 기준)
			float PointerYInGauge = (1.f - CurrentGaugeValue) * (GaugeHeight - PointerHeight);

			//게이지 배경 위젯의 Y위치를 기준으로 최종 절대 Y좌표 계산
			float AbsolutePointerY = GaugeBackgroundSlot->GetPosition().Y + PointerYInGauge;

			float SomeOffSet = 20.0f; // 게이지 바와 커서 사이의 간격
			float PointerX = GaugeBackgroundSlot->GetPosition().X + GaugeWidth + SomeOffSet;

			//커서가 게이지 바 영영의 Y축 범위 내에 있도록 클램프
			AbsolutePointerY = FMath::Clamp(AbsolutePointerY, GaugeBackgroundSlot->GetPosition().Y, GaugeBackgroundPos.Y + GaugeHeight - PointerHeight);

			PointerSlot->SetPosition(FVector2D(PointerX, AbsolutePointerY));
		}
	}
}


// Called every frame
void UVerticalTimingGaugeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsGaugeActive)
	{
		CurrentGaugeValue = GaugeDirection * GaugeSpeed * DeltaTime;

		if (CurrentGaugeValue >= 1.0f)
		{
			CurrentGaugeValue = 1.0f;
			GaugeDirection = -1.0f;
		}
		else if (CurrentGaugeValue <= 0.0f)
		{
			CurrentGaugeValue = 0.0f;
			GaugeDirection = 1.0f;
		}

		UpdatePointerUI(); //커서 위치 업데이트
	}
}

