// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/BNTarotCardAttributeSet.h"
#include "GameplayEffectExtension.h"

#include "BaseGamePlayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BNMonoCharacter.h"

UBNTarotCardAttributeSet::UBNTarotCardAttributeSet()
{
	InitSpeedBuffDuration(5.f);
	InitMoveSpeed(200.f);
	InitMoveSpeedMultiplier(1.3f);
}

void UBNTarotCardAttributeSet::Init(UAbilitySystemComponent* InASC)
{
	AttributeAbilitySystemComponent = InASC;
}


void UBNTarotCardAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayTagContainer GrantedTags;
	Data.EffectSpec.GetAllGrantedTags(GrantedTags);

	// if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	// {
	// 	ChangeMoveSpeed();
	// }
}

void UBNTarotCardAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetMoveSpeedAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSetCheck1"));
		if (AttributeAbilitySystemComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("AttributeSetCheck2"));
			FGameplayTagContainer ActiveTags;
			AttributeAbilitySystemComponent->GetOwnedGameplayTags(ActiveTags);

			if (ActiveTags.HasTag(BaseGamePlayTags::Effect_Item_TarotCard_MoveSpeed))
			{
				UE_LOG(LogTemp, Warning, TEXT("AttributeSetCheck3"));
				ChangeMoveSpeed(NewValue);
			}
		}
	}
}

void UBNTarotCardAttributeSet::ChangeMoveSpeed(float NewSpeed)
{
	//float NewSpeed = GetMoveSpeed();
	if (AActor* Owner = GetOwningActor())
	{
		if (ACharacter* Character = Cast<ABNMonoCharacter>(Owner))
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
		}
	}
}
