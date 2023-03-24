// Copyright 2022 CQUnreal. All Rights Reserved.

#include "StudioAndroidPermissionPrivate.h"
#include "Modules/ModuleManager.h"


/** Logging definitions */
DEFINE_LOG_CATEGORY(LogStudioAndroidPermission);

class FStudioAndroidPermissionModule : public IModuleInterface
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

IMPLEMENT_MODULE(FStudioAndroidPermissionModule, StudioAndroidPermission);
