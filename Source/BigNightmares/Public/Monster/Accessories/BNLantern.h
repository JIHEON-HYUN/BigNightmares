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
	ABNLantern();

protected:
	// 랜턴의 외형을 표시할 스켈레탈 메시 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* LanternMesh;

	// 랜턴의 불빛을 표현할 스포트라이트 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* SpotLight;
};
