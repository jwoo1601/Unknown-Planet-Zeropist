// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UprisingServerTarget : TargetRules
{
	public UprisingServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

        ExtraModuleNames.AddRange(
            new string[]
            {
                "Uprising",
                "UprisingServer"
            });
	}
}
