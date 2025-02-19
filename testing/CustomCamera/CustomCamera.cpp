#include "CustomCamera.h"
#include "Modules/ModuleManager.h"

void FCustomCameraModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("CustomCamera Module Loaded"));
}

void FCustomCameraModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("CustomCamera Module Unloaded"));
}

IMPLEMENT_MODULE(FCustomCameraModule, CustomCamera)