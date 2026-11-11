// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Ashes : ModuleRules
{
	public Ashes(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"SlateCore",
			"HTTP",
			"Json",
			"JsonUtilities",
			"NavigationSystem",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "DeveloperSettings", "Sockets" });

		PublicIncludePaths.AddRange(new string[] {
			"Ashes",
			"Ashes/Player",
			"Ashes/Gameplay",
			"Ashes/Dialogue",
			"Ashes/Core",
			"Ashes/Data",
			"Ashes/UI",
			"Ashes/Variant_Platforming",
			"Ashes/Variant_Platforming/Animation",
			"Ashes/Variant_Combat",
			"Ashes/Variant_Combat/AI",
			"Ashes/Variant_Combat/Animation",
			"Ashes/Variant_Combat/Gameplay",
			"Ashes/Variant_Combat/Interfaces",
			"Ashes/Variant_Combat/UI",
			"Ashes/Variant_SideScrolling",
			"Ashes/Variant_SideScrolling/AI",
			"Ashes/Variant_SideScrolling/Gameplay",
			"Ashes/Variant_SideScrolling/Interfaces",
			"Ashes/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
