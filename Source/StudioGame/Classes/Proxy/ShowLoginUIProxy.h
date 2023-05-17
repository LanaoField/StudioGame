// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "Online/CoreOnline.h"
#else
#include "UObject/CoreOnline.h"
#endif
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ShowLoginUIProxy.generated.h"

class APlayerController;
struct FOnlineError;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowLoginUIResult, APlayerController*, PlayerController);

UCLASS(MinimalAPI)
class UShowLoginUIProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UShowLoginUIProxy(const FObjectInitializer& ObjectInitializer);

	// Called when there is a successful query
	UPROPERTY(BlueprintAssignable)
	FShowLoginUIResult OnSuccess;

	// Called when there is an unsuccessful query
	UPROPERTY(BlueprintAssignable)
	FShowLoginUIResult OnFailure;

	// Shows the login UI for the currently active online subsystem, if the subsystem supports a login UI.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online")
	static UShowLoginUIProxy* ShowLoginUI(UObject* WorldContextObject, class APlayerController* InPlayerController, FName InSubsystemName);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// End of UBlueprintAsyncActionBase interface

private:
	// Internal callback when the login UI closes, calls out to the public success/failure callbacks
	void OnShowLoginUICompleted(FUniqueNetIdPtr UniqueId, int LocalUserNum, const FOnlineError& Error);

	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;

	// The online subsystem name
	FName SubsystemName;
};
