// Copyright 2022 CQUnreal. All Rights Reserved.

#include "StudioGameplayAbilitiesPrivate.h"
#include "Modules/ModuleManager.h"

class FStudioGameplayAbilitiesModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{

	}

	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_MODULE(FStudioGameplayAbilitiesModule, StudioGameplayAbilities);
