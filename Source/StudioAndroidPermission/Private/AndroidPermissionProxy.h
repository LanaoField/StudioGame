// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h" 
#include "UObject/ScriptMacros.h"
#include "Delegates/Delegate.h"
#include "AndroidPermissionCallbackProxy.h"
#include "AndroidPermissionProxy.generated.h"


UCLASS()
class STUDIOANDROIDPERMISSION_API UAndroidPermissionProxy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="AndroidPermission")
	FAndroidPermissionDynamicDelegate OnPermissionsGrantedDynamicDelegate;

	FAndroidPermissionDelegate OnPermissionsGrantedDelegate;
	
	static UAndroidPermissionProxy* GetInstance();
};
