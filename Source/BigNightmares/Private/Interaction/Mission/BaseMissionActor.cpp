// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Mission/BaseMissionActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NaniteSceneProxy.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseMissionActor::ABaseMissionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//서버에 복제 허용
	bReplicates = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActorMesh"));
	SetRootComponent(ActorMesh);

	OverlapBox = CreateDefaultSubobject<UBoxComponent>("OverlapBox");
	OverlapBox->SetupAttachment(GetRootComponent());

	// 해당 컴포넌트가 모든 충돌 채널에 대한 충돌 응답을 설정 **지금의 경우 무시**
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 해당 컴포넌트가 특정 충돌 채널에 대한 충돌 응답을 설정 **Pawn에 대한 충돌은 OverLap을 감지**
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
}

// Called when the game starts or when spawned
void ABaseMissionActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//OverlapBox Component에 BeginOverlap이 발생하면 ABaseMissionActor에 정의된 OnBoxBeginOverlap 함수를 발동.
		OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseMissionActor::OnBoxBeginOverlap);
	}
	
}

void ABaseMissionActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FGameplayEffectContextHandle ContextHandle = OtherASC->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = OtherASC->MakeOutgoingSpec(OverlapEffect, 1.f, ContextHandle);
		OtherASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		Destroy();
	}
}

UBoxComponent* ABaseMissionActor::GetOverlapComponent() const
{
	return OverlapBox;
}

UStaticMeshComponent* ABaseMissionActor::GetStaticMeshComponent() const
{
	return ActorMesh;
}

TSubclassOf<UGameplayEffect> ABaseMissionActor::GetOverlapEffect() const
{
	return OverlapEffect;
}
