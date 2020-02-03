// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Uprising : ModuleRules
{
	public Uprising(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });

        PublicIncludePaths.AddRange(
            new string[]
            {
                "Uprising/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
                "Uprising/Private"
            });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
            });

        if (UEBuildConfiguration.bWithServerCode == true)
        {
            PublicDependencyModuleNames.Add("UprisingServer");
            CircularlyReferencedDependentModules.Add("UprisingServer");
        }
	}
}
