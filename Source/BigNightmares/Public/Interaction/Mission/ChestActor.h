// ChestActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "ChestActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;

/**
 * 상호작용의 핵심 기능에만 집중하여 완전히 재구성된 상자 액터입니다.
 * 복잡한 로직을 모두 제거하고, 가장 안정적인 방식으로 작동하도록 설계되었습니다.
 */
UCLASS()
class BIGNIGHTMARES_API AChestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	AChestActor();

	// 이 상자가 '진짜 상자'인지 식별하는 변수입니다. ChestSpawner 등 외부에서 설정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Chest")
	bool bIsTheCorrectChest = false;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// [추가] Tick 함수를 다시 사용하도록 오버라이드합니다.
	virtual void Tick(float DeltaTime) override;

	// 컴포넌트들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PromptWidget;

private:
	// 플레이어가 상자 범위에 들어왔을 때 호출될 함수입니다.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 플레이어가 상자 범위에서 나갔을 때 호출될 함수입니다.
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// InteractionInterface의 함수를 구현합니다.
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	
	// [추가] 모든 클라이언트에게 흔들림 효과를 재생하라고 명령하는 멀티캐스트 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayShakeEffect();

	// [추가] 흔들림 애니메이션 관련 변수들
	bool bIsShaking;
	float ShakeDuration;
	float ShakeIntensity;
	float ShakeTimer;
	FVector OriginalMeshLocation;
};
