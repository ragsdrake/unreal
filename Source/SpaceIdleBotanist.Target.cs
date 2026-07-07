// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceIdleBotanistTarget : TargetRules
{
	public SpaceIdleBotanistTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("SpaceIdleBotanist");
	}
}
