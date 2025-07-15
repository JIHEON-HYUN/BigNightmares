// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BNBaseCharacter.generated.h"

class UBNTarotCardAttributeSet;
class UBNBaseAbilitySystemComponent;
class UBNBaseAttributeSet;
UCLASS()
class BIGNIGHTMARES_API ABNBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABNBaseCharacter();

protected:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	UBNBaseAbilitySystemComponent* GetBNBaseAbilitySystemComponent() const;
	//UBNTarotCardAttributeSet* GetBNBaseAttributeSet() const;

public:
	template<typename T>
	T* GetAttributeSet() const
	{
		static_assert(std::is_base_of<UBNBaseAttributeSet, T>::value, "BaseCharacter : T must derive from UBNBaseAttributeSet");
		return nullptr;
	}
};
