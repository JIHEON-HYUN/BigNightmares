// BNMissionTriggerActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "BNMissionTriggerActor.generated.h"

class UBoxComponent;
class ABNMonoCharacter;
class ABNBaseCharacter;
class UStaticMeshComponent;

UCLASS()
class BIGNIGHTMARES_API ABNMissionTriggerActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ABNMissionTriggerActor();

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

private:
	// [추가] 플레이어가 상호작용 영역에 들어왔을 때 호출될 함수입니다.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// [추가] 플레이어가 상호작용 영역에서 나갔을 때 호출될 함수입니다.
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TWeakObjectPtr<ABNMonoCharacter> FirstInteractor;
};
