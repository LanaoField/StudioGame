// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StudioGameBlueprintLibrary.generated.h"

UENUM(BlueprintType)
enum class EModifyScreenOrientation : uint8
{
	Unknown,

	/** Portrait orientation (the display is taller than it is wide). */
	Portrait,

	/** The orientation is landscape with the home button at the right side */
	LandscapeLeft,

	/** The orientation is landscape with the home button at the left side */
	LandscapeRight,

	/** Use any orientation the device normally supports, based on the device orientation sensor. */
	Sensor, 
};

/**
 * 
 */
UCLASS()
class STUDIOGAME_API UStudioGameBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void LoadTexture(const FString& ImageRelativePath, UTexture2DDynamic*& Texture);

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
	static float GetAverageFPS();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static float GetAverageMS();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static int64 GetApplicationMemoryUsage();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static bool IsVideoRecording();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void StartVideoRecording(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void StopVideoRecording(const FText& Title, const FText& Comment);

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static int64 GetUnixTimestamp();

	UFUNCTION(BlueprintCallable, Category = StudioGame)
	static void SetDeviceOrientation(EModifyScreenOrientation InScreenOrientation);
};