using UnrealBuildTool;

public class gasTarget : TargetRules
{
    public gasTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "PlayerCharacter", "WeaponSystem", "CustomCamera" });
    }
}