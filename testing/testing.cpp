// testing.cpp
#include "testing.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FTestingModule, testing, "testing");

void FTestingModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("testing module startup"));
}

void FTestingModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("testing module shutdown"));
}
