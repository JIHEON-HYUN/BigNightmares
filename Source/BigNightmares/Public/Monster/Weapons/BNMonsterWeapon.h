// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNMonsterWeapon.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;

UCLASS()
class BIGNIGHTMARES_API ABNMonsterWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABNMonsterWeapon();

	// [에러 수정] 시작: BNHunterCharacter에서 호출할 수 있도록 함수들을 선언합니다.
	/** 근접 공격용 콜리전을 활성화합니다. 애니메이션 노티파이에서 호출됩니다. */
	void ActivateMeleeCollision();
	
	/** 근접 공격용 콜리전을 비활성화합니다. 애니메이션 노티파이에서 호출됩니다. */
	void DeactivateMeleeCollision();
	// [에러 수정] 끝

protected:
	virtual void BeginPlay() override;

	/** 무기의 외형을 나타내는 스켈레탈 메시 컴포넌트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// [에러 수정] 시작: 근접 공격 판정을 위한 콜리전 컴포넌트를 추가합니다.
	/** 근접 공격 판정을 위한 박스 콜리전 컴포넌트입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	TObjectPtr<UBoxComponent> MeleeAttackCollision;

	/** 오버랩(충돌) 이벤트가 발생했을 때 호출될 함수입니다. */
	UFUNCTION()
	void OnMeleeAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// [에러 수정] 끝

private:
	/** 한 번의 공격 스윙 동안 피해를 입은 액터들의 목록입니다. 중복 피해를 방지합니다. */
	TArray<TObjectPtr<AActor>> HitActors;
};
