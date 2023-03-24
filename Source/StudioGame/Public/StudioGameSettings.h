// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StudioGameSettings.generated.h"

/**
 * UStudioGameSettings
 */
UCLASS(Config = StudioGame, DefaultConfig, Meta = (DisplayName = "StudioGame"))
class STUDIOGAME_API UStudioGameSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, Category = Build)
	bool bEnableGameplayAbilitiesEx;

	UPROPERTY(Config, EditAnywhere, Category = Build)
	bool bEnableWebBrowserEx;

	UPROPERTY(Config, EditAnywhere, Category = Build)
	bool bEnableAndroidPermissionEx;
};
