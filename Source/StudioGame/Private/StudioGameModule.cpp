// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "StudioGamePrivate.h"
#include "StudioGameSettings.h"
#include "Styling/StudioGameCoreStyle.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#define LOCTEXT_NAMESPACE "FStudioGameModule"

/** Logging definitions */
DEFINE_LOG_CATEGORY(LogStudioGame);

class FStudioGameModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		RegisterSettings();

#if ENABLE_GAMEPLAYABILITIESEX
		FModuleManager::Get().LoadModuleChecked(TEXT("StudioGameplayAbilities"));
#endif

#if ENABLE_WEBBROWSEREX
		FModuleManager::Get().LoadModuleChecked(TEXT("StudioWebBrowser"));
#endif

#if ENABLE_ANDROIDPERMISSIONEX
		FModuleManager::Get().LoadModuleChecked(TEXT("StudioAndroidPermission"));
#endif
	}

	virtual void ShutdownModule() override
	{
		UnregisterSettings();
	}

	void RegisterSettings()
	{
#if WITH_EDITOR
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (!SettingsModule)
			return;

		const auto Section = SettingsModule->RegisterSettings("Project", "Plugins", "StudioGame",
			LOCTEXT("StudioGameEditorSettingsName", "StudioGame"),
			LOCTEXT("StudioGameEditorSettingsDescription", "StudioGame Settings"),
			GetMutableDefault<UStudioGameSettings>());
		Section->OnModified().BindRaw(this, &FStudioGameModule::OnSettingsModified);
#endif
	}

	void UnregisterSettings()
	{
#if WITH_EDITOR
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule)
			SettingsModule->UnregisterSettings("Project", "Plugins", "StudioGame");
#endif
	}

	bool OnSettingsModified()
	{
		return true;
	}
};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStudioGameModule, StudioGame)