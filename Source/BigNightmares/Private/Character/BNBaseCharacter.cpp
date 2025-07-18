// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseCharacter.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "GameFramework/PlayerState/BNPlayerState.h"

// Sets default values
ABNBaseCharacter::ABNBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;

	//BNBaseAbilitySystemComponent->
}

// Called when the game starts or when spawned
void ABNBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ABNBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ABNBaseCharacter::GetAbilitySystemComponent() const
{
	return GetBNBaseAbilitySystemComponent();
}

void ABNBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ABNPlayerState* PS = GetPlayerState<ABNPlayerState>())
	{
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->InitAbilityActorInfo(PS,this);
		}
	}
}

UBNBaseAbilitySystemComponent* ABNBaseCharacter::GetBNBaseAbilitySystemComponent() const
{
	if (const ABNPlayerState* PS = GetPlayerState<ABNPlayerState>())
	{
		return PS->GetBNBaseAbilitySystemComponent();
	}
	return nullptr;
}
