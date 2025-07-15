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
	// 총을 발사하는 함수입니다. GameplayAbility에서 이 함수를 호출하게 됩니다.
	UFUNCTION(BlueprintCallable, Category = "Weapon|Gun")
	void Fire();

protected:
	// 총알이 발사될 위치를 나타내는 소켓의 이름입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	FName MuzzleSocketName = TEXT("Muzzle");

	// 발사 시 재생될 파티클 효과입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	UParticleSystem* MuzzleFlashEffect;

	// 발사 시 재생될 사운드입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Gun")
	USoundBase* FireSound;
};
