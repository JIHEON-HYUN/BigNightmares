// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapons/BNMonsterWeapon.h"


ABNMonsterWeapon::ABNMonsterWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// 메시 컴포넌트를 생성하고 루트 컴포넌트로 설정합니다.
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}
