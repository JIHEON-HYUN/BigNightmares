// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/BNTarotCardAttributeSet.h"
#include "GameplayEffectExtension.h"

#include "BaseGamePlayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/BNMonoCharacter.h"

UBNTarotCardAttributeSet::UBNTarotCardAttributeSet()
{
	InitSpeedBuffDuration(5.f);
	InitBaseMoveSpeed(200.f);
	InitMoveSpeedUpMultiplier(10.f); // 1.3f 를 기본으로
	InitMoveSpeedDownMultiplier(0.5f);
}

void UBNTarotCardAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBNTarotCardAttributeSet, BaseMoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBNTarotCardAttributeSet, MoveSpeedUpMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBNTarotCardAttributeSet, SpeedBuffDuration, COND_None, REPNOTIFY_Always);
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

	UE_LOG(LogTemp, Warning, TEXT("PostGameplayEffectExecute called on AttributeSet %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("Attribute Changed: %s = %f"),
		*Data.EvaluatedData.Attribute.GetName(), Data.EvaluatedData.Magnitude);
	AActor* Owner = GetOwningActor();
	UE_LOG(LogTemp, Warning, TEXT("GetOwningActor(): %p"), Owner);

	
	if (Data.EvaluatedData.Attribute == GetBaseMoveSpeedAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSetCheck1"));
		
		const float FinalSpeed = BaseMoveSpeed.GetCurrentValue();

		UE_LOG(LogTemp, Warning, TEXT("MoveSpeed 최종 계산 결과: %f"), FinalSpeed);
		//SetBaseMoveSpeed(FinalSpeed);

		ChangeMoveSpeed(FinalSpeed);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Evaluated Attribute: %s"), *Data.EvaluatedData.Attribute.GetName());
}

void UBNTarotCardAttributeSet::ChangeMoveSpeed(float NewSpeed, int32 Count)
{
	//float NewSpeed = GetMoveSpeed();
	UE_LOG(LogTemp, Warning, TEXT("ChangeMoveSpeed NewSpeed: %f"), NewSpeed);
	if (AActor* Avatar = GetOwningActor())
	{
		if (ABNMonoCharacter* MonoCharacter = Cast<ABNMonoCharacter>(Avatar))
		{
			if (MonoCharacter->GetCharacterMovement())
			{
				MonoCharacter->Client_ApplyMoveSpeed(NewSpeed);
				UE_LOG(LogTemp, Warning, TEXT("최종 속도 캐릭터에 적용: %f"), NewSpeed);
			}
		}
	}
}

void UBNTarotCardAttributeSet::OnRep_BaseMoveSpeed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBNTarotCardAttributeSet, BaseMoveSpeed, OldSpeed);
}

void UBNTarotCardAttributeSet::OnRep_MoveSpeedUpMultiplier(const FGameplayAttributeData& UpMulti)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBNTarotCardAttributeSet, MoveSpeedUpMultiplier, UpMulti);
}

void UBNTarotCardAttributeSet::OnRep_SpeedBuffDuration(const FGameplayAttributeData& BuffDuration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBNTarotCardAttributeSet, SpeedBuffDuration, BuffDuration);
}
