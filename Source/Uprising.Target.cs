// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UprisingTarget : TargetRules
{
	public UprisingTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

        ExtraModuleNames.Add("Uprising");

        if (bBuildEditor)
        {
            ExtraModuleNames.AddRange(
                new string[]
                {
                    "UprisingEditor"
                });
        }
	}
}
