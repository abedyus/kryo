#include "PlayerCharacter.h"
#include "Modules/ModuleManager.h"

void FPlayerCharacterModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("PlayerCharacter Module Loaded"));
}

void FPlayerCharacterModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("PlayerCharacter Module Unloaded"));
}

IMPLEMENT_MODULE(FPlayerCharacterModule, PlayerCharacter)