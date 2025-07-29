// BNThinmanCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseMonster.h"
#include "Interfaces/LightSensitive.h"
#include "GameplayTagContainer.h"
#include "BNThinmanCharacter.generated.h"

class UAnimMontage;

UCLASS()
class BIGNIGHTMARES_API ABNThinmanCharacter : public ABNBaseMonster, public ILightSensitive
{
	GENERATED_BODY()

public:
	ABNThinmanCharacter();

	virtual void OnHitByLight_Implementation(bool bIsLit) override;

	// 상태 진입 함수들
	virtual void EnterIdleState() override;
	virtual void EnterChasingState() override;
	virtual void EnterAttackingState() override;

	// 외부에서 AttackMontage를 안전하게 가져갈 수 있도록 함수를 추가합니다.
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }

protected:
	virtual void BeginPlay() override;

	// 애니메이션 노티파이가 호출할 함수입니다.
	UFUNCTION(BlueprintCallable)
	void AnimNotify_KillAllPlayers();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|State")
	FGameplayTag StunnedByLightTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 이동 속도 관련 변수들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float ChaseSpeed = 100.0f;
};
