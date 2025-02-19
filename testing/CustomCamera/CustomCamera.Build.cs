using UnrealBuildTool;
using System.IO; // Added so that 'Path' is recognized

public class CustomCamera : ModuleRules
{
    public CustomCamera(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Core dependencies
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "GameplayCameras", // For camera shakes
            "UMG",             // For UI integration (if needed)
            "Slate",
            "SlateCore"
        });

        // Private dependencies
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "RenderCore",
            "RHI",
            "AudioMixer",   // For audio effects
            "MediaAssets",  // For post-processing
            "CinematicCamera" // For advanced camera features
        });

        // Editor-specific dependencies
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",    // For editor-specific functionality
                "EditorStyle"
            });
        }

        // Include paths
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
    }
}
