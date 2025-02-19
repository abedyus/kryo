using UnrealBuildTool;
using System.IO;

public class testing : ModuleRules // Renamed to "Testing" for consistency (optional)
{
    public testing(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		
        bEnableExceptions = true;
        bUseUnity = false; // Recommended for ONNX compatibility

        // Core engine dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTasks",
            "UMG",
            "GameplayCameras",
            "AIModule",
            "NNERuntimeORT", // Use NNERuntimeORT instead of NeuralNetworkInference
            "NNERuntimeRDG"  // Optional: Add if you need RDG-based neural network inference
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "OpenCV", // Required for computer vision workflows
            "RenderCore",
            "RHI"
        });

        // ONNX Runtime configuration
        string OnnxRuntimePath = Path.Combine(ModuleDirectory, "../../ThirdParty/OnnxRuntime");

        // Library and DLL paths (assumes both are in the "lib" folder; adjust if necessary)
        string OnnxRuntimeLibPath = Path.Combine(OnnxRuntimePath, "lib");
        string OnnxRuntimeBinPath = Path.Combine(OnnxRuntimePath, "lib"); // Update if DLLs are in a separate folder

        // Add library paths
        PublicSystemLibraryPaths.Add(OnnxRuntimeLibPath);

        // Add the library file
        string OnnxRuntimeLibFile = Path.Combine(OnnxRuntimeLibPath, "onnxruntime.lib");
        if (File.Exists(OnnxRuntimeLibFile))
        {
            PublicAdditionalLibraries.Add(OnnxRuntimeLibFile);
        }
        else
        {
            throw new BuildException($"ONNX Runtime library not found at {OnnxRuntimeLibFile}");
        }

        // Header includes
        PublicIncludePaths.Add(Path.Combine(OnnxRuntimePath, "include"));
        PrivateIncludePaths.Add(Path.Combine(OnnxRuntimePath, "include"));

        // DLL handling for Windows
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string OnnxRuntimeDllFile = Path.Combine(OnnxRuntimeBinPath, "onnxruntime.dll");
            if (File.Exists(OnnxRuntimeDllFile))
            {
                RuntimeDependencies.Add("$(BinaryOutputDir)/onnxruntime.dll", OnnxRuntimeDllFile);
                PublicDelayLoadDLLs.Add("onnxruntime.dll");
            }
            else
            {
                throw new BuildException($"ONNX Runtime DLL not found at {OnnxRuntimeDllFile}");
            }
        }

        // Essential compiler definitions
        PublicDefinitions.AddRange(new string[] {
            "WITH_ONNXRUNTIME=1",
            "ONNXRUNTIME_VERSION=12010",
            "_SCL_SECURE_NO_WARNINGS",
            "NOMINMAX" // Prevent conflicts with Windows.h
        });

        // Optimization flags
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        UndefinedIdentifierWarningLevel = WarningLevel.Off; // Replaces bEnableUndefinedIdentifierWarnings
    }
}
