// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Accessories/BNLantern.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"

ABNLantern::ABNLantern()
{
	PrimaryActorTick.bCanEverTick = false;

	// [수정됨] 스켈레탈 메시 컴포넌트를 생성하고 루트로 설정합니다.
	LanternMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LanternMesh"));
	SetRootComponent(LanternMesh);

	// 스포트라이트 컴포넌트를 생성하고 루트(메시)에 붙입니다.
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(5000.f);
	SpotLight->SetOuterConeAngle(25.f);
}
