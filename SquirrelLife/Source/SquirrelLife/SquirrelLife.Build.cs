// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SquirrelLife : ModuleRules
{
	public SquirrelLife(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SquirrelLife",
			"SquirrelLife/Variant_Platforming",
			"SquirrelLife/Variant_Platforming/Animation",
			"SquirrelLife/Variant_Combat",
			"SquirrelLife/Variant_Combat/AI",
			"SquirrelLife/Variant_Combat/Animation",
			"SquirrelLife/Variant_Combat/Gameplay",
			"SquirrelLife/Variant_Combat/Interfaces",
			"SquirrelLife/Variant_Combat/UI",
			"SquirrelLife/Variant_SideScrolling",
			"SquirrelLife/Variant_SideScrolling/AI",
			"SquirrelLife/Variant_SideScrolling/Gameplay",
			"SquirrelLife/Variant_SideScrolling/Interfaces",
			"SquirrelLife/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
