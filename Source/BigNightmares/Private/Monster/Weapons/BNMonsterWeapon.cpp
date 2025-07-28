// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapons/BNMonsterWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/BNMonoCharacter.h" // 플레이어 캐릭터를 참조하기 위해 포함합니다.
#include "Kismet/GameplayStatics.h"

ABNMonsterWeapon::ABNMonsterWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// [에러 수정] 시작: 근접 공격용 콜리전 컴포넌트를 생성하고 초기화합니다.
	MeleeAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttackCollision"));
	MeleeAttackCollision->SetupAttachment(RootComponent);
	MeleeAttackCollision->SetCollisionProfileName(TEXT("NoCollision")); 
	MeleeAttackCollision->SetGenerateOverlapEvents(false);
	// [에러 수정] 끝
}

void ABNMonsterWeapon::BeginPlay()
{
	Super::BeginPlay();
	// [에러 수정] 시작: 오버랩 이벤트에 함수를 바인딩합니다.
	MeleeAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ABNMonsterWeapon::OnMeleeAttackOverlap);
	// [에러 수정] 끝
}

// [에러 수정] 시작: BNHunterCharacter에서 호출할 함수들의 내용을 구현합니다.
void ABNMonsterWeapon::ActivateMeleeCollision()
{
	HitActors.Empty();
	MeleeAttackCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MeleeAttackCollision->SetGenerateOverlapEvents(true);
	UE_LOG(LogTemp, Log, TEXT("Melee Attack Collision ENABLED."));
}

void ABNMonsterWeapon::DeactivateMeleeCollision()
{
	MeleeAttackCollision->SetCollisionProfileName(TEXT("NoCollision"));
	MeleeAttackCollision->SetGenerateOverlapEvents(false);
	UE_LOG(LogTemp, Log, TEXT("Melee Attack Collision DISABLED."));
}

void ABNMonsterWeapon::OnMeleeAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// [로그 추가] 오버랩 이벤트가 누구와 발생했는지 항상 기록합니다.
	UE_LOG(LogTemp, Log, TEXT("Weapon overlapped with: %s"), *OtherActor->GetName());
	
	// 자기 자신이나, 무기의 주인(헌터), 이미 맞은 액터는 무시합니다.
	if (OtherActor == this || OtherActor == GetOwner() || HitActors.Contains(OtherActor))
	{
		// [로그 추가] 공격이 왜 무시되었는지 이유를 기록합니다.
		UE_LOG(LogTemp, Warning, TEXT("Attack ignored for %s (Self, Owner, or Already Hit)."), *OtherActor->GetName());
		return;
	}

	// 충돌한 액터가 플레이어인지 확인합니다.
	ABNMonoCharacter* PlayerCharacter = Cast<ABNMonoCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		// [로그 추가] 플레이어에게 성공적으로 피해를 입혔음을 기록합니다.
		UE_LOG(LogTemp, Error, TEXT("SUCCESSFUL HIT! Player '%s' will be killed."), *PlayerCharacter->GetName());
		
		UE_LOG(LogTemp, Warning, TEXT("Melee Weapon hit Player: %s"), *PlayerCharacter->GetName());
		// 플레이어의 사망 처리 함수를 호출합니다.
		PlayerCharacter->HandleLethalHit();
		// 중복 타격을 방지하기 위해 맞은 액터 목록에 추가합니다.
		HitActors.Add(PlayerCharacter);
	}
	else
	{
		// [로그 추가] 플레이어가 아닌 다른 액터와 충돌했음을 기록합니다.
		UE_LOG(LogTemp, Log, TEXT("Hit '%s', but it is not a player character."), *OtherActor->GetName());
	}
}
// [에러 수정] 끝
