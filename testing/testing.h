// testing.h
#pragma once

#include "Modules/ModuleManager.h"

class FTestingModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
