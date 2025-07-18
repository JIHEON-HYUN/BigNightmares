// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMissionActor.generated.h"

class UGameplayEffect;
class UBoxComponent;

UCLASS()
class BIGNIGHTMARES_API ABaseMissionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMissionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
private:
	UPROPERTY(VisibleDefaultsOnly, meta =(AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ActorMesh;

	UPROPERTY(VisibleDefaultsOnly, meta =(AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> OverlapBox;

	UPROPERTY(EditAnywhere, Category="Custom Values|Effects")
	TSubclassOf<UGameplayEffect> OverlapEffect;

protected:
	UFUNCTION()
	UBoxComponent* GetOverlapComponent() const;

	UFUNCTION()
	UStaticMeshComponent* GetStaticMeshComponent() const;

	UFUNCTION()
	TSubclassOf<UGameplayEffect> GetOverlapEffect() const;
};
