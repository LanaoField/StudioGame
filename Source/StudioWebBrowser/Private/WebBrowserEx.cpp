// Copyright 2022 CQUnreal. All Rights Reserved.

#include "WebBrowserEx.h"
#include "SWebBrowser.h"


/////////////////////////////////////////////////////
// UWebBrowserEx

UWebBrowserEx::UWebBrowserEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UWebBrowserEx::Reload()
{
	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->Reload();
	}
}

void UWebBrowserEx::StopLoad()
{
	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->StopLoad();
	}
}