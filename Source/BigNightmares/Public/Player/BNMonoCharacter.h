// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BNBaseCharacter.h"
#include "BNMonoCharacter.generated.h"

class UDataAsset_InputConfig_Player;
class USpringArmComponent;
class UCameraComponent;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class BIGNIGHTMARES_API ABNMonoCharacter : public ABNBaseCharacter
{
	GENERATED_BODY()

	ABNMonoCharacter();

#pragma region Camera
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
#pragma  endregion

#pragma region Inputs
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CharacterData", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig_Player> InputConfigDataAsset;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);	
#pragma endregion
};
