// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceIdleBotanistEditorTarget : TargetRules
{
	public SpaceIdleBotanistEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("SpaceIdleBotanist");
	}
}
