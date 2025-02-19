#include "WeaponSystem.h"
#include "Modules/ModuleManager.h"

void FWeaponSystemModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("WeaponSystem Module Loaded"));
}

void FWeaponSystemModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("WeaponSystem Module Unloaded"));
}

IMPLEMENT_MODULE(FWeaponSystemModule, WeaponSystem)