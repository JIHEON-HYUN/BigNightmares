// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BigNightmares : ModuleRules
{
	public BigNightmares(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"GameplayAbilities", "GameplayTags", "GameplayTasks", "AnimGraphRuntime",
			"OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam", 
			"AIModule", "NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"BigNightmares", "Voice"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
