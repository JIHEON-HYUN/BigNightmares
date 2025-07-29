// ChestActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "ChestActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class ABNBaseCharacter;

UCLASS()
class BIGNIGHTMARES_API AChestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	AChestActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Chest")
	bool bIsTheCorrectChest = false;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PromptWidget;

private:
	// [수정] 파라미터를 인터페이스 원본과 동일하게 AActor* 로 변경
	virtual void Interact_Implementation(AActor* InteractingActor) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayShakeEffect();

	bool bIsShaking;
	float ShakeDuration;
	float ShakeIntensity;
	float ShakeTimer;
	FVector OriginalMeshLocation;
};
