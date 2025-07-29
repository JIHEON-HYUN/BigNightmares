// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMissionActor.generated.h"

class UCapsuleComponent;
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
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta =(AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ActorMesh;

	UPROPERTY(VisibleDefaultsOnly, meta =(AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> OverlapBox;

	UPROPERTY(VisibleDefaultsOnly, meta =(AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> OverlapCapsule;

	UPROPERTY(EditAnywhere, Category="Custom Values|Effects")
	TSubclassOf<UGameplayEffect> OverlapEffect;

protected:
	UFUNCTION()
	UBoxComponent* GetBoxOverlapComponent() const;

	UFUNCTION()
	UCapsuleComponent* GetCapsuleOverlapComponent() const;

	UFUNCTION()
	UStaticMeshComponent* GetStaticMeshComponent() const;

	UFUNCTION()
	TSubclassOf<UGameplayEffect> GetOverlapEffect() const;
};
