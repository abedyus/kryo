using UnrealBuildTool;
using System.IO; // Make sure Path is available

public class PlayerCharacter : ModuleRules
{
    public PlayerCharacter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
            "WeaponSystem",  // Dependency on WeaponSystem module
            "CustomCamera",  // Dependency on CustomCamera module
            "EnhancedInput"  // Added dependency for Enhanced Input system
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        // Include paths
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
    }
}
