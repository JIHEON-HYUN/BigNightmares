// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Accessories/BNLantern.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"

// 생성자
ABNLantern::ABNLantern()
{
	// 틱 비활성화
	PrimaryActorTick.bCanEverTick = false;

	// 랜턴 메시 컴포넌트 생성
	LanternMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LanternMesh"));
	// 루트 컴포넌트로 설정
	SetRootComponent(LanternMesh);

	// 스포트라이트 컴포넌트 생성
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	// 루트 컴포넌트에 부착
	SpotLight->SetupAttachment(RootComponent);
	// 불빛 강도 설정
	SpotLight->SetIntensity(5000.f);
	// 외부 콘 각도 설정
	SpotLight->SetOuterConeAngle(25.f);
}
