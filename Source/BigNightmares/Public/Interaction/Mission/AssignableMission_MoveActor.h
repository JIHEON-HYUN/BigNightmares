// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssignableMission_MoveActor.generated.h"

class USphereComponent;

UCLASS()
class BIGNIGHTMARES_API AAssignableMission_MoveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssignableMission_MoveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> ActorMesh;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> SphereComponent;

public:
	UFUNCTION()
	void SettingCollision();
};
