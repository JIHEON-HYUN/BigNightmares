 // Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstance/BNHunterAnimInstance.h"
#include "Monster/BNHunterCharacter.h"

 void UBNHunterAnimInstance::NativeInitializeAnimation()
 {
  Super::NativeInitializeAnimation();

  if (OwningMonsterCharacter)
  {
   OwningHunterCharacter = Cast<ABNHunterCharacter>(OwningMonsterCharacter);
  }

  if (!OwningHunterCharacter)
  {
   UE_LOG(LogTemp, Warning, TEXT("UMonsterAnimInstance::NativeInitializeAnimation - OwningHunterCharacter is null!"));
  }
 }

 void UBNHunterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
 {
  Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

  if (!OwningHunterCharacter)
  {
   return;
  }
 }
