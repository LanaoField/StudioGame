// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StudioGameBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STUDIOGAME_API UStudioGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void ReadContentImage(FString ImageRelativePath, UTexture2DDynamic*& Texture);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void RequestExitGame(bool bForce);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static bool GetDiskTotalAndFreeSpace(const FString& InPath, int64& TotalNumberOfBytes, int64& NumberOfFreeBytes);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static FString GetDeviceMakeAndModel();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static FString GetDeviceId();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void ClipboardCopy(const FString& Str);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void ClipboardPaste(FString& Dest);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static float GetFPS();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static float GetMS();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static int64 GetApplicationMemoryUsage();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static bool IsVideoRecording();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void StartVideoRecording(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void StopVideoRecording(const FText& Title, const FText& Comment);
};