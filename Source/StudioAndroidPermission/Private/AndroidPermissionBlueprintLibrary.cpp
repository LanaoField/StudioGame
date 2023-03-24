// Copyright 2022 CQUnreal. All Rights Reserved.

#include "AndroidPermissionBlueprintLibrary.h"
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionProxy.h"


bool UAndroidPermissionBlueprintLibrary::CheckPermission(const FString& Permission)
{
	return UAndroidPermissionFunctionLibrary::CheckPermission(Permission);
}

void OnPermissionsGranted(const TArray<FString>& Permissions, const TArray<bool>& GrantResults)
{
	AsyncTask(ENamedThreads::GameThread, [Permissions, GrantResults]()
		{
			UAndroidPermissionProxy* AndroidPermissionProxy = UAndroidPermissionProxy::GetInstance();
			if (AndroidPermissionProxy != nullptr)
			{
				#if ENGINE_MAJOR_VERSION >= 5
				AndroidPermissionProxy->OnPermissionsGrantedDelegate.Broadcast(Permissions, GrantResults);
				AndroidPermissionProxy->OnPermissionsGrantedDynamicDelegate.Broadcast(Permissions, GrantResults);
				#else				
				AndroidPermissionProxy->OnPermissionsGrantedDelegate.ExecuteIfBound(Permissions, GrantResults);
				AndroidPermissionProxy->OnPermissionsGrantedDynamicDelegate.Broadcast(Permissions, GrantResults);
				#endif
			}
		});
}

UAndroidPermissionProxy* UAndroidPermissionBlueprintLibrary::AcquirePermissions(const TArray<FString>& Permissions)
{
	UAndroidPermissionCallbackProxy* AndroidPermissionCallbackProxy = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
	if (AndroidPermissionCallbackProxy != nullptr)
	{
	#if ENGINE_MAJOR_VERSION >= 5
		AndroidPermissionCallbackProxy->OnPermissionsGrantedDelegate.AddStatic(&OnPermissionsGranted);
	#else
		AndroidPermissionCallbackProxy->OnPermissionsGrantedDelegate.BindStatic(&OnPermissionsGranted);
	#endif
	}

	return UAndroidPermissionProxy::GetInstance();
}