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
	ABNHunterCharacter();

	virtual void ActivateMonster() override;

	// BTTask_ExecuteAttack에서 몽타주 정보를 가져갈 수 있도록 Getter 함수를 추가합니다.
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	
	// [수정] BTTask에서 호출할 수 있도록 상태 변경 함수들을 public으로 옮깁니다.
	virtual void EnterIdleState() override;
	virtual void EnterChasingState() override;
	virtual void EnterAttackingState() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABNMonsterWeapon> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ABNMonsterWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponAttachSocketName = TEXT("r_hand_ikinema_0_bneSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> DefaultLanternClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> EquippedLantern;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName LanternAttachSocketName = TEXT("l_hand_ikinema_0_bneSocket");
};
