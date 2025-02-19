using UnrealBuildTool;
using System.Collections.Generic;

public class gasEditorTarget : TargetRules
{
    public gasEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "PlayerCharacter", "WeaponSystem", "CustomCamera" });
    }
}