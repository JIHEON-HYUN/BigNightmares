// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "BNHunterCharacter.generated.h"

class ABNMonsterWeapon;
class AActor;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNHunterCharacter : public ABNBaseMonster
{
	GENERATED_BODY()

public:
	// 생성자
	ABNHunterCharacter();

	// 몬스터 활성화 함수 (오버라이드)
	virtual void ActivateMonster() override;

	// 공격 몽타주 Getter
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	
	// 대기 상태 진입 (오버라이드)
	virtual void EnterIdleState() override;
	// 추격 상태 진입 (오버라이드)
	virtual void EnterChasingState() override;
	// 공격 상태 진입 (오버라이드)
	virtual void EnterAttackingState() override;

	UFUNCTION()
	void AnimNotify_ExecuteGuaranteedHit();

	UFUNCTION()
	void AnimNotify_ActivateMeleeCollision();

	UFUNCTION()
	void AnimNotify_DeactivateMeleeCollision();
	
protected:
	// [추가] 공격 몽타주를 모든 클라이언트에 재생시키는 Multicast 함수입니다.
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage();

	// 게임 시작 또는 스폰 시 호출
	virtual void BeginPlay() override;

	// 걷기 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 100.0f;

	// 추격 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 150.0f;

	// 기본 무기 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABNMonsterWeapon> DefaultWeaponClass;

	// 장착된 무기 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ABNMonsterWeapon> EquippedWeapon;

	// 무기 부착 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponAttachSocketName = TEXT("r_hand_ikinema_0_bneSocket");

	// 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 기본 랜턴 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> DefaultLanternClass;

	// 장착된 랜턴 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> EquippedLantern;

	// 랜턴 부착 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName LanternAttachSocketName = TEXT("l_hand_ikinema_0_bneSocket");

	/** 플레이어를 멈추게 할 시간(초)입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ImmobilizeDuration = 0.7f;
};
