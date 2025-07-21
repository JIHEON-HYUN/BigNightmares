// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/VerticalTimingGaugeComponent.h"

#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameState/BNGameState.h"
#include "GameFramework/PlayerController/BNPlayerController.h"
#include "UI/InGame/BNInGameWidget.h"

// Sets default values for this component's properties
UVerticalTimingGaugeComponent::UVerticalTimingGaugeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bIsLogicActive = false;

	// 컴포넌트 자체 복제 허용
	SetIsReplicatedByDefault(true);

	//bAlwaysRelevant = true; //멀리 있어도 항상 복제 되도록 설정 (AActor 자료형에서 가능)

	GaugeSpeed = 0.5f; //커서 움직임 속도 (초당 0.5f로 이동) / 한번 성공할 때 마다 값이 커지게
	CurrentGaugeValue  = 0.0f; //0아래, 1위
	GaugeDirection = 1.0f; //초기에 아래에서 위로 움직이면서 시작,

	GreenZoneStart = 0.25f; //0.25은 기본값, 적용될 값은 서버에서 랜덤으로 결정할 것
	GreenZoneLength = 0.25f; //블루프린트에서 설정될 값
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
	
	DOREPLIFETIME(UVerticalTimingGaugeComponent, CurrentGaugeValue);
	DOREPLIFETIME(UVerticalTimingGaugeComponent, bIsLogicActive);
}

void UVerticalTimingGaugeComponent::RequestStartGauge(ABNPlayerController* BNPlayerController)
{
	if (!GetWorld()) return;

	if (!GaugeID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GaugeID is not set for this UVerticalTimingGaugeComponent. Cannot request start."));
		return;
	}

	if (bIsLogicActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("GaugeID '%s' is already active"), *GaugeID.ToString());
		return;
	}
	
	// 클라이언트든 서버든 (리스닝 서버의 호스트 클라이언트 부분 포함),
	// 게이지 시작 요청은 항상 서버 RPC를 통해 이루어집니다.
	Server_RequestStartGaugeInternal(GaugeID, BNPlayerController);
}

// Server_RequestStartGaugeInternal RPC 유효성 검사
bool UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Validate(FGuid InGaugeID, ABNPlayerController* BNPlayerController)
{
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

	//BNPlayerController의 유효성
	if (!IsValid(BNPlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Not Invalid BNPlayerController : %s"), *BNPlayerController->GetName());
		return false;
	}
	
	return true;
}

// Server_RequestStartGaugeInternal RPC 구현 (서버에서 실행)
void UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Implementation(FGuid InGaugeID, ABNPlayerController* BNPlayerController)
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;

	if (!IsValid(BNPlayerController)) return;

	if (this->GaugeID !=  InGaugeID) return;

	if (bIsLogicActive) return;
	
	//서버에서 GreenZone의 위치를 랜덤 결정
	GreenZoneStart = FMath::FRandRange(0.0f, 1.0f - GreenZoneLength);
	//해당 변수는 값이 바뀌는 순간 ReplicatedUsing에 의해 클라에 복제

	BNPlayerController->Client_StartGaugeUI(this);

	bIsLogicActive = true;

	UE_LOG(LogTemp, Log, TEXT("Server: Gauge ID '%s' UI/logic initiated for Player %s (targeting specific client)."), 
		*GaugeID.ToString(), *BNPlayerController->GetName());
}

void UVerticalTimingGaugeComponent::HandleGaugeInput()
{
	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABNPlayerController* LocalBNPC = Cast<ABNPlayerController>(LocalPC);

	if (!IsValid(LocalBNPC) || LocalBNPC->ActiveGaugeComponent.Get() != this) return;
	
	EVerticalGaugeResult Result = EVerticalGaugeResult::EVGR_Fail;

	if (CurrentGaugeValue >= GreenZoneStart && CurrentGaugeValue <= (GreenZoneStart + GreenZoneLength))
	{
		Result = EVerticalGaugeResult::EVGR_Success;
	}
	else
	{
		Result = EVerticalGaugeResult::EVGR_Fail;
	}

	if (LocalBNPC->IsLocalController())
	{
		LocalBNPC->Server_NotifyGaugeFinished(GaugeID, Result);
	}
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

	if (InGaugeID.IsValid()) return false;
	
	//서버가 해당 GaugeID에 대한 정보를 가지고 있는지 확인
	if (this->GaugeID !=  InGaugeID)  // 이 컴포넌트의 실제 GaugeID와 RPC로 받은 InGaugeID가 일치하는지
	{
		UE_LOG(LogTemp, Warning, TEXT("Validate: Mismatched GaugeID. RPC GaugeID: %s, Component ID: %s"), *InGaugeID.ToString(), *this->GaugeID.ToString());
		return false;
	}
	
	return true;
}

void UVerticalTimingGaugeComponent::Server_RequestEndGaugeInternal_Implementation(FGuid InGaugeID,
	EVerticalGaugeResult Result)
{

	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;

	if (GaugeID != InGaugeID) return;
	if (!bIsLogicActive) return;

	bIsLogicActive = false; //서버 게이지 로직 비활성화
	CurrentGaugeValue = 0.0f; //초기화
	GaugeDirection = 1.0f; //초기화
	
	OnGaugeFinished.Broadcast(Result);
}

void UVerticalTimingGaugeComponent::OnRep_GreenZoneLocation()
{
	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABNPlayerController* LocalBNPC = Cast<ABNPlayerController>(LocalPC);
	
	if (IsValid(LocalBNPC) && LocalBNPC->ActiveGaugeComponent.Get() == this)
	{
		LocalBNPC->UpdateGreenZoneUI(GreenZoneStart, GreenZoneLength);
	}
}

// Called every frame
void UVerticalTimingGaugeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority() && bIsLogicActive)
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
	}
}

