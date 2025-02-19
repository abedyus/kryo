using UnrealBuildTool;

public class MotionMatching : ModuleRules
{
    public MotionMatching(ReadOnlyTargetRules Target) : base(Target)
    {
        // Use explicit or shared PCHs (adjust as necessary)
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // Public include paths for module headers
        PublicIncludePaths.AddRange(new string[] {
            "MotionMatching/Public"
        });
        
        // Private include paths for internal headers
        PrivateIncludePaths.AddRange(new string[] {
            "MotionMatching/Private",
        });
        
        // List public dependencies that you statically link with here
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore"
        });
        
        // List private dependencies that you statically link with here
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });
        
        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
