// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HeroEditor : ModuleRules
{
	public HeroEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PublicIncludePaths.AddRange(
            new string[]
            {
                "HeroEditor/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "HeroEditor/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Uprising",
                "UprisingEditor"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd"
            });

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            });
	}
}
