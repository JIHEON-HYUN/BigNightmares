// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNMonsterGameplayTags.h"
#include "GameplayTagsManager.h"

FBNMonsterGameplayTags FBNMonsterGameplayTags::GameplayTags;

void FBNMonsterGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.Character_Monster_Dormant = Manager.AddNativeGameplayTag(
		FName("Character.Monster.Dormant"),
		FString("Monster is in a dormant state.")
	);

	GameplayTags.Character_Monster_Active = Manager.AddNativeGameplayTag(
		FName("Character.Monster.Active"),
		FString("Monster is in an active state.")
	);

	GameplayTags.Character_Monster_Active_Idle = Manager.AddNativeGameplayTag(
		FName("Character.Monster.Active.Idle"),
		FString("Active monster is idle.")
	);

	GameplayTags.Character_Monster_Active_Chasing = Manager.AddNativeGameplayTag(
		FName("Character.Monster.Active.Chasing"),
		FString("Active monster is chasing a target.")
	);

	GameplayTags.Character_Monster_Active_Attacking = Manager.AddNativeGameplayTag(
		FName("Character.Monster.Active.Attacking"),
		FString("Active monster is attacking.")
	);
	
	Manager.DoneAddingNativeTags();
}