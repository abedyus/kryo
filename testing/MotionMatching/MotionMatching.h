#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * The primary game module for MotionMatching.
 */
class FMotionMatchingModule : public IModuleInterface
{
public:
    /** Called when the module is loaded into memory */
    virtual void StartupModule() override;

    /** Called before the module is unloaded */
    virtual void ShutdownModule() override;
};
