// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/AssetManager.h"
#include "AssetManagerEx.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FAsyncLoadDelegate, class UPrimaryDataAsset*, PrimaryDataAsset);

/**
 * Game implementation of asset manager, overrides functionality and stores game-specific types
 * It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic
 * This is used by setting AssetManagerClassName in DefaultEngine.ini
 */
UCLASS()
class STUDIOGAME_API UAssetManagerEx : public UAssetManager
{
	GENERATED_BODY()

public:
	UAssetManagerEx()
		: UAssetManager()
	{

	}

	virtual void StartInitialLoading() override;

	/** Returns the current AssetManager object */
	static UAssetManagerEx& Get();

	/**
	 * Synchronously loads an Item subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UFUNCTION(BlueprintCallable, Category = Asset)
	static class UPrimaryDataAsset* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);

	/**
	 * Asynchronous loads an Item subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param AsyncLoadDelegate The after loading, call back the delegate
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UFUNCTION(BlueprintCallable, Category = Asset)
	static void AsyncLoadItem(const FPrimaryAssetId& PrimaryAssetId, FAsyncLoadDelegate AsyncLoadDelegate, bool bLogWarning = true);
};

