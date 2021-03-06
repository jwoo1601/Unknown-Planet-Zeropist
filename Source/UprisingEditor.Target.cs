// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UprisingEditorTarget : TargetRules
{
	public UprisingEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.AddRange(
            new string[]
            {
                "Uprising",
                "UprisingEditor"
            });
	}
}
