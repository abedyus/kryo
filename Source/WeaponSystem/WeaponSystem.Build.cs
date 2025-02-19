using UnrealBuildTool;
using System.IO;

public class WeaponSystem : ModuleRules
{
    public WeaponSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Networking",
            "Sockets",
            "UMG",
            "Slate",
            "SlateCore",
            "RenderCore",  // Needed for canvas rendering types
            "RHI"          // Optional: add if further rendering types are needed
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        // Add the Public and Private directories explicitly
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
    }
}

