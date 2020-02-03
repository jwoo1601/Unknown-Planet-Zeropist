// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UprisingServer : ModuleRules
{
	public UprisingServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" /*, "InputCore" */ });

        PublicIncludePaths.AddRange(
            new string[]
            {
                "UprisingServer/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "UprisingServer/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Uprising"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
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
