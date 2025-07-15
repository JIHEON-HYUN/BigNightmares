// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "BNHunterCharacter.generated.h"

class ABNMonsterWeapon;
class AActor;

/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNHunterCharacter : public ABNBaseMonster
{
	GENERATED_BODY()

public:
	ABNHunterCharacter();

	// 몬스터를 활성화시키는 함수입니다.
	virtual void ActivateMonster() override;
	
protected:
	// 게임이 시작될 때 호출됩니다.
	virtual void BeginPlay() override;

	// 블루프린트에서 헌터의 기본 걷기 속도를 설정할 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 100.0f;

	// 블루프린트에서 헌터의 추격 속도를 설정할 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 150.0f;

	// --- 무기 관련 ---
	// 이 헌터가 장착할 무기의 종류입니다. 블루프린트에서 설정합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABNMonsterWeapon> DefaultWeaponClass;

	// 현재 장착하고 있는 무기 액터의 인스턴스입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ABNMonsterWeapon> EquippedWeapon;

	// 무기를 장착할 손의 소켓 이름입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponAttachSocketName = TEXT("r_hand_ikinema_0_bneSocket");

	// --- 랜턴 관련 ---
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> DefaultLanternClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> EquippedLantern;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName LanternAttachSocketName = TEXT("l_hand_ikinema_0_bneSocket");
	
	virtual void EnterIdleState() override;
	virtual void EnterChasingState() override;
	virtual void EnterAttackingState() override;
	
private:

	// [추가됨] 몬스터 자동 활성화를 위한 타이머 핸들입니다.
	FTimerHandle ActivateTimerHandle;
};
