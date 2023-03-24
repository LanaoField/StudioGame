// Copyright 2022 CQUnreal. All Rights Reserved.

#include "StudioWebBrowserPrivate.h"
#include "Modules/ModuleManager.h"

class FStudioWebBrowserModule : public IModuleInterface
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

IMPLEMENT_MODULE(FStudioWebBrowserModule, StudioWebBrowser);
