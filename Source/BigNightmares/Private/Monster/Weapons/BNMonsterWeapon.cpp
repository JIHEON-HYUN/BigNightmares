// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapons/BNMonsterWeapon.h"

// 생성자
ABNMonsterWeapon::ABNMonsterWeapon()
{
	// 틱 비활성화
	PrimaryActorTick.bCanEverTick = false;

	// 웨폰 메시 컴포넌트 생성
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	// 루트 컴포넌트로 설정
	SetRootComponent(WeaponMesh);
}
