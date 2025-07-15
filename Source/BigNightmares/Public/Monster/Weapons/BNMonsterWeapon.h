// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNMonsterWeapon.generated.h"

UCLASS()
class BIGNIGHTMARES_API ABNMonsterWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	ABNMonsterWeapon();

	// 무기의 외형을 표시할 스켈레탈 메시 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;
};
