// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/Input/DataAsset_InputConfig_Player.h"

UInputAction* UDataAsset_InputConfig_Player::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FBaseInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	
	return nullptr;
}
