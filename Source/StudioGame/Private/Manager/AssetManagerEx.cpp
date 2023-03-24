// Copyright Epic Games, Inc. All Rights Reserved.

#include "Manager/AssetManagerEx.h"
#include "Engine/DataAsset.h"
#include "StudioGamePrivate.h"


DECLARE_CYCLE_STAT(TEXT("ForceLoadItem"), STAT_ForceLoadItem, STATGROUP_StudioGame);
DECLARE_CYCLE_STAT(TEXT("AsyncLoadItem"), STAT_AsyncLoadItem, STATGROUP_StudioGame);

UAssetManagerEx& UAssetManagerEx::Get()
{
	UAssetManagerEx* This = Cast<UAssetManagerEx>(GEngine->AssetManager);
	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogStudioGame, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be AssetManagerEx!"));
		return *NewObject<UAssetManagerEx>(); // never calls this
	}
}

void UAssetManagerEx::StartInitialLoading()
{
	Super::StartInitialLoading();
}

UPrimaryDataAsset* UAssetManagerEx::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	SCOPE_CYCLE_COUNTER(STAT_ForceLoadItem);

	FSoftObjectPath ItemPath = UAssetManagerEx::Get().GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UPrimaryDataAsset* LoadedItem = Cast<UPrimaryDataAsset>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogStudioGame, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}

void UAssetManagerEx::AsyncLoadItem(const FPrimaryAssetId& PrimaryAssetId, FAsyncLoadDelegate AsyncLoadDelegate, bool bLogWarning)
{
	SCOPE_CYCLE_COUNTER(STAT_AsyncLoadItem);

	FSoftObjectPath ItemPath = UAssetManagerEx::Get().GetPrimaryAssetPath(PrimaryAssetId);
	
	UAssetManagerEx::Get().GetStreamableManager().RequestAsyncLoad(ItemPath, [ItemPath, PrimaryAssetId, AsyncLoadDelegate, bLogWarning](void) {
		UPrimaryDataAsset* LoadedItem = Cast<UPrimaryDataAsset>(ItemPath.ResolveObject());
		if (bLogWarning && LoadedItem == nullptr)
		{
			UE_LOG(LogStudioGame, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
		}
		AsyncLoadDelegate.ExecuteIfBound(LoadedItem);
	});
}