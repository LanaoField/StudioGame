// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AndroidPermissionBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STUDIOANDROIDPERMISSION_API UAndroidPermissionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StudioGame, meta = (DisplayName = "Check Android Permission"))
	static bool CheckPermission(const FString& Permission);

	UFUNCTION(BlueprintCallable, Category = StudioGame, meta = (DisplayName = "Request Android Permissions"))
	static class UAndroidPermissionProxy* AcquirePermissions(const TArray<FString>& Permissions);
};