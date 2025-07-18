// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "BNRifle.generated.h"

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNRifle : public ABNMonsterWeapon
{
	GENERATED_BODY()

public:
	// 총 발사 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon|Gun")
	void Fire();

protected:
	// 총구 소켓 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	FName MuzzleSocketName = TEXT("Muzzle");

	// 총구 화염 파티클
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	UParticleSystem* MuzzleFlashEffect;

	// 발사 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	USoundBase* FireSound;
};
