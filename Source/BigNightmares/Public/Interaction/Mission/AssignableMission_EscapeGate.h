// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssignableMission_EscapeGate.generated.h"

class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class BIGNIGHTMARES_API AAssignableMission_EscapeGate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssignableMission_EscapeGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(EditAnywhere,	meta=(AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere,	meta=(AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> EscapeTrigger;

	UPROPERTY(EditAnywhere,	meta=(AllowPrivateAccess = true))
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GameCleared();

	void Server_GameCleared();
};
