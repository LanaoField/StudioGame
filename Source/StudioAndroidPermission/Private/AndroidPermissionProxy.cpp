// Copyright 2022 CQUnreal. All Rights Reserved.

#include "AndroidPermissionProxy.h"
#include "StudioAndroidPermissionPrivate.h"


static UAndroidPermissionProxy *pProxy = NULL;

UAndroidPermissionProxy* UAndroidPermissionProxy::GetInstance()
{
	if (!pProxy) {
		pProxy = NewObject<UAndroidPermissionProxy>();
		pProxy->AddToRoot();
	}
	UE_LOG(LogStudioAndroidPermission, Log, TEXT("UAndroidPermissionProxy::GetInstance"));
	return pProxy;
}