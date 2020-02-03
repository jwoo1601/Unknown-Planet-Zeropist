// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UprisingEditor : ModuleRules
{
	public UprisingEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PublicIncludePaths.AddRange(
            new string[]
            {
                "UprisingEditor/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "UprisingEditor/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Uprising"
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
