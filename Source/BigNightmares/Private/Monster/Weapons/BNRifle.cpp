// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapons/BNRifle.h"
#include "Kismet/GameplayStatics.h"

void ABNRifle::Fire()
{
	// 총구 위치에 발사 이펙트(파티클, 사운드)를 재생합니다.
	if (MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEffect, WeaponMesh, MuzzleSocketName);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// TODO: 여기에 실제 총알(Projectile)을 발사하는 로직을 추가할 수 있습니다.
}
