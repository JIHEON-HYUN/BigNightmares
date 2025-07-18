// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapons/BNRifle.h"
#include "Kismet/GameplayStatics.h"

// 총 발사 함수
void ABNRifle::Fire()
{
	// 총구 화염 이펙트 유효성 확인
	if (MuzzleFlashEffect)
	{
		// 이펙트 스폰
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEffect, WeaponMesh, MuzzleSocketName);
	}
	// 발사 사운드 유효성 확인
	if (FireSound)
	{
		// 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// TODO: 여기에 실제 총알(Projectile)을 발사하는 로직을 추가할 수 있습니다.
}
