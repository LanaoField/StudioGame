// Copyright 2022 CQUnreal. All Rights Reserved.

#include "StudioGameBlueprintLibrary.h"
#include "Engine.h"
#include "RHIResources.h"
#include "Engine/Texture2DDynamic.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "PlatformFeatures.h"
#include "VideoRecordingSystem.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformApplicationMisc.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#if !UE_SERVER
static void WriteRawDataToTexture_RenderThread(FTexture2DDynamicResource* TextureResource, const TArray<uint8>& RawData, bool bUseSRGB = true)
{
	check(IsInRenderingThread());

	FTexture2DRHIRef TextureRHI = TextureResource->GetTexture2DRHI();

	int32 Width = TextureRHI->GetSizeX();
	int32 Height = TextureRHI->GetSizeY();

	uint32 DestStride = 0;
	uint8* DestData = reinterpret_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

	for (int32 y = 0; y < Height; y++)
	{
		uint8* DestPtr = &DestData[(Height - 1 - y) * DestStride];

		const FColor* SrcPtr = &((FColor*)(RawData.GetData()))[(Height - 1 - y) * Width];
		for (int32 x = 0; x < Width; x++)
		{
			*DestPtr++ = SrcPtr->B;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->R;
			*DestPtr++ = SrcPtr->A;
			SrcPtr++;
		}
	}

	RHIUnlockTexture2D(TextureRHI, 0, false, false);
}
#endif

void UStudioGameBlueprintLibrary::LoadTexture(const FString& ImageRelativePath, UTexture2DDynamic*& Texture)
{
#if !UE_SERVER
	FString ImageFullPath = FPaths::Combine(*FPaths::ProjectContentDir(), *ImageRelativePath);

	TArray<uint8> Result;

	FFileHelper::LoadFileToArray(Result, *ImageFullPath);

	if (Result.Num() > 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[2] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
		};

		for (auto ImageWrapper : ImageWrappers)
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Result.GetData(), Result.Num()))
			{
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 24
				TArray<uint8> RawData;
				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
				#else 
				const TArray<uint8>* RawData = NULL;
				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
				#endif
				{
					Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());

					if (Texture)
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						#if ENGINE_MAJOR_VERSION > 4
						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
						#else
						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
						#endif

						#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION > 24
						ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
						[TextureResource, RawData](FRHICommandListImmediate& RHICmdList)
						{
							WriteRawDataToTexture_RenderThread(TextureResource, RawData);
						});
						#else
						TArray<uint8> RawDataCopy = *RawData;
						ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
						[TextureResource, RawDataCopy](FRHICommandListImmediate& RHICmdList)
						{
							WriteRawDataToTexture_RenderThread(TextureResource, RawDataCopy);
						});
						#endif
					}
				}
			}
		}
	}
#endif
}

void UStudioGameBlueprintLibrary::RequestExitGame(bool bForce)
{
	FPlatformMisc::RequestExit(bForce);
}

bool UStudioGameBlueprintLibrary::GetDiskTotalAndFreeSpace(const FString& InPath, int64& TotalNumberOfBytes, int64& NumberOfFreeBytes)
{
	return FPlatformMisc::GetDiskTotalAndFreeSpace(InPath, *(uint64*)&TotalNumberOfBytes, *(uint64*)&NumberOfFreeBytes);
}

FString UStudioGameBlueprintLibrary::GetDeviceMakeAndModel()
{
	return FPlatformMisc::GetDeviceMakeAndModel();
}

FString UStudioGameBlueprintLibrary::GetDeviceId()
{
#if UE_EDITOR
	return TEXT("Development Editor");
#else
	return FPlatformMisc::GetDeviceId();
#endif
}

void UStudioGameBlueprintLibrary::ClipboardCopy(const FString& Str)
{
	FPlatformApplicationMisc::ClipboardCopy(*Str);
}

void UStudioGameBlueprintLibrary::ClipboardPaste(FString& Dest)
{
	FPlatformApplicationMisc::ClipboardPaste(Dest);
}

float UStudioGameBlueprintLibrary::GetAverageFPS()
{
	extern ENGINE_API float GAverageFPS;
	return GAverageFPS;
}

float UStudioGameBlueprintLibrary::GetAverageMS()
{
	extern ENGINE_API float GAverageMS;
	return GAverageMS;
}

int64 UStudioGameBlueprintLibrary::GetApplicationMemoryUsage()
{
	uint32 CurrentProcessId = FPlatformProcess::GetCurrentProcessId();

	SIZE_T OutMemoryUsage;
	FPlatformProcess::GetApplicationMemoryUsage(CurrentProcessId, &OutMemoryUsage);

	return (int64)OutMemoryUsage;
}

bool UStudioGameBlueprintLibrary::IsVideoRecording()
{
	IVideoRecordingSystem* VideoRecordSystem = IPlatformFeaturesModule::Get().GetVideoRecordingSystem();
	return VideoRecordSystem->GetRecordingState() == EVideoRecordingState::Recording;
}

void UStudioGameBlueprintLibrary::StartVideoRecording(const FString& Name)
{
	IVideoRecordingSystem* VideoRecordSystem = IPlatformFeaturesModule::Get().GetVideoRecordingSystem();
	VideoRecordSystem->EnableRecording(true);
	FVideoRecordingParameters Parameters;
	Parameters.bAutoStart = true;
	Parameters.bAutoContinue = true;
	Parameters.RecordingLengthSeconds = 3600;
	VideoRecordSystem->NewRecording(*Name, Parameters);
}

void UStudioGameBlueprintLibrary::StopVideoRecording(const FText& Title, const FText& Comment)
{
	IVideoRecordingSystem* VideoRecordSystem = IPlatformFeaturesModule::Get().GetVideoRecordingSystem();
	VideoRecordSystem->FinalizeRecording(true, Title, Comment);
}

int64 UStudioGameBlueprintLibrary::GetUnixTimestamp()
{
	int64 NowTicks = FDateTime::UtcNow().GetTicks();
	int64 UnixTicks = FDateTime(1970, 1, 1).GetTicks();

	return (NowTicks - UnixTicks) / ETimespan::TicksPerMillisecond;
}

void UStudioGameBlueprintLibrary::SetDeviceOrientation(EModifyScreenOrientation InScreenOrientation)
{
#if PLATFORM_ANDROID
	int32 Value = -1;
	switch (InScreenOrientation)
	{
	case EModifyScreenOrientation::Unknown:
		Value = -1;
		break;
	case EModifyScreenOrientation::Portrait:
		Value = 1;
		break;
	case EModifyScreenOrientation::LandscapeLeft:
		Value = 0;
		break;
	case EModifyScreenOrientation::LandscapeRight:
		Value = 8;
		break;
	case EModifyScreenOrientation::Sensor:
		Value = 4;
		break;
	}

	if (Value > -1)
	{
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, FJavaWrapper::AndroidThunkJava_SetOrientation, Value);
		}
	}
#endif
}
