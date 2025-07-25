// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssignableMission_MoveActor.generated.h"

class USphereComponent;
class UNiagaraComponent;

// 미션 액터가 파괴될 때 호출될 Delegate
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionActorDestroyed, AActor*, DestroyedActor);

UCLASS()
class BIGNIGHTMARES_API AAssignableMission_MoveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssignableMission_MoveActor();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraComponent> MoveNiagara;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraComponent> FinishNiagara;

public:
	UFUNCTION()
	void SettingCollision();

	UFUNCTION()
	void ChangeNiagara();

	//FinishNiagara의 활성화 상태 복제
	UPROPERTY(ReplicatedUsing = OnRep_IsFinishNiagaraActive)
	bool bIsFinishNiagaraActive;

	UFUNCTION()
	void OnRep_IsFinishNiagaraActive();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetFinishNiagaraState(bool bNewFinishState);

	// UPROPERTY(BlueprintAssignable, Category = "Mission")
	// FOnMissionActorDestroyed OnMissionActorDestroyed;
};
