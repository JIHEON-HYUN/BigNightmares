// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNLantern.generated.h"

class USkeletalMeshComponent;
class USpotLightComponent;

UCLASS()
class BIGNIGHTMARES_API ABNLantern : public AActor
{
	GENERATED_BODY()
	
public:
	// 생성자
	ABNLantern();

protected:
	// 랜턴 외형 스켈레탈 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* LanternMesh;

	// 랜턴 불빛 스포트라이트 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* SpotLight;
};
