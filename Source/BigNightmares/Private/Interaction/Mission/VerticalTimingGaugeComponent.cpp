// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/VerticalTimingGaugeComponent.h"

// Sets default values for this component's properties
UVerticalTimingGaugeComponent::UVerticalTimingGaugeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	//bAlwaysRelevant = true; //멀리 있어도 항상 복제 되도록 설정 (AActor 자료형에서 가능)
	
	

	// ...
}


// Called when the game starts
void UVerticalTimingGaugeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UVerticalTimingGaugeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UVerticalTimingGaugeComponent::RequestStartGauge()
{
}

void UVerticalTimingGaugeComponent::Client_StartGaugeUI_Implementation()
{
}

void UVerticalTimingGaugeComponent::HandleGaugeInput()
{
}

void UVerticalTimingGaugeComponent::Client_EndGaugeUI_Implementation(EVerticalGaugeResult Result)
{
}

void UVerticalTimingGaugeComponent::OnRep_GreenZoneLocation()
{
}

void UVerticalTimingGaugeComponent::UpdateGreenZoneUI()
{
}

void UVerticalTimingGaugeComponent::UpdatePointerUI()
{
}

void UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Implementation(FGuid InGaugeID)
{
}

bool UVerticalTimingGaugeComponent::Server_RequestStartGaugeInternal_Validate(FGuid InGaugeID)
{
	return false;
}

void UVerticalTimingGaugeComponent::Server_RequestEndGaugeInternal_Implementation(FGuid InGaugeID,
	EVerticalGaugeResult Result)
{
}

bool UVerticalTimingGaugeComponent::Server_RequestEndGaugeInternal_Validate(FGuid InGaugeID,
	EVerticalGaugeResult Result)
{
	return false;
}


// Called every frame
void UVerticalTimingGaugeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

