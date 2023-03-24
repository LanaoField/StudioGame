// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "WebBrowser.h"
#include "WebBrowserEx.generated.h"


/**
 * UWebBrowserEx
 */
UCLASS()
class STUDIOWEBBROWSER_API UWebBrowserEx : public UWebBrowser
{
	GENERATED_UCLASS_BODY()

public:
	/** Reload the current page. */
	UFUNCTION(BlueprintCallable, Category="Web Browser")
	void Reload();

	/** Stop loading the page. */
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
	void StopLoad();
};
