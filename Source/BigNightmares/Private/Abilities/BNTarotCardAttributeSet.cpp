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
	InitMoveSpeedUpMultiplier(10.f); // 1.3f 를 기본으로
	InitMoveSpeedDownMultiplier(0.5f);
}

void UBNTarotCardAttributeSet::Init(UBNBaseAbilitySystemComponent* InASC)
{
	AttributeAbilitySystemComponent = InASC;
	UE_LOG(LogTemp, Warning, TEXT("Init called, ASC ptr: %p"), AttributeAbilitySystemComponent.Get());
}

void UBNTarotCardAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//TODO(NOTE): 나중에 일정시간 적용후 해제가 아닌경우 여기 사용
	FGameplayTagContainer GrantedTags;
	Data.EffectSpec.GetAllGrantedTags(GrantedTags);
}

void UBNTarotCardAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSetCheck1"));
		UE_LOG(LogTemp, Warning, TEXT("ASC Ptr  :  %p"), AttributeAbilitySystemComponent.Get())
		
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
	UE_LOG(LogTemp, Warning, TEXT("ChangeMoveSpeed NewSpeed: %f"), NewSpeed);
	if (AActor* Owner = AttributeAbilitySystemComponent->GetAvatarActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeMoveSpeed Owner: %s"), *GetNameSafe(Owner));
		UE_LOG(LogTemp, Warning, TEXT("Owner class: %s"), Owner ? *Owner->GetClass()->GetName() : TEXT("nullptr"));
		if (ACharacter* Character = Cast<ABNMonoCharacter>(Owner))
		{
			UE_LOG(LogTemp, Warning, TEXT("Old MaxWalkSpeed: %f"), Character->GetCharacterMovement()->MaxWalkSpeed);
			Character->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			UE_LOG(LogTemp, Warning, TEXT("New MaxWalkSpeed: %f"), Character->GetCharacterMovement()->MaxWalkSpeed);
		}
	}
}
