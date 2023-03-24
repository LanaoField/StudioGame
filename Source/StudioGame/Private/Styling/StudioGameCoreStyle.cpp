// Copyright Epic Games, Inc. All Rights Reserved.

#include "Styling/StudioGameCoreStyle.h"
#include "Styling/SlateStyle.h"


#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

// This is to fix the issue that SlateStyleMacros like IMAGE_BRUSH look for RootToContentDir but Style->RootToContentDir is how the core style is set up
#define RootToContentDir Style->RootToContentDir

/* Static initialization
 *****************************************************************************/

TSharedPtr< ISlateStyle > FStudioGameCoreStyle::Instance = nullptr;

class FStudioGameCoreStyleSet
	: public FSlateStyleSet
{
public:
	FStudioGameCoreStyleSet(const FName& InStyleSetName)
		: FSlateStyleSet(InStyleSetName)

		// These are the colors that are updated by the user style customizations
	{
	}
};

/* FStudioGameCoreStyle static functions
 *****************************************************************************/

TSharedRef<ISlateStyle> FStudioGameCoreStyle::Create()
{
	TSharedRef<FStudioGameCoreStyleSet> Style = MakeShareable(new FStudioGameCoreStyleSet("StudioGameCoreStyle"));
	Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FString CanaryPath = RootToContentDir(TEXT("Checkerboard"), TEXT(".png"));

	if (!FPaths::FileExists(CanaryPath))
	{
		// Checkerboard is the default brush so we check for that. No slate fonts are required as those will fall back properly
		UE_LOG(LogSlate, Warning, TEXT("FStudioGameCoreStyle assets not detected, skipping FStudioGameCoreStyle initialization"));

		return Style;
	}

	FDoubleSliderStyle DoubleSliderStyle = FDoubleSliderStyle()
		.SetNormalBarImage(FSlateColorBrush(FColor::White))
		.SetHoveredBarImage(FSlateColorBrush(FColor::White))
		.SetDisabledBarImage(FSlateColorBrush(FLinearColor::Gray))
		.SetNormalThumbImage(IMAGE_BRUSH("Common/Button", FVector2D(8.0f, 14.0f)))
		.SetHoveredThumbImage(IMAGE_BRUSH("Common/Button", FVector2D(8.0f, 14.0f)))
		.SetDisabledThumbImage(IMAGE_BRUSH("Common/Button_Disabled", FVector2D(8.0f, 14.0f)))
		.SetBarThickness(2.0f);
	Style->Set("DoubleSlider", DoubleSliderStyle);

	return Style;
}

void FStudioGameCoreStyle::ResetToDefault()
{
	SetStyle(FStudioGameCoreStyle::Create());
}

/* FSlateThrottleManager implementation
 *****************************************************************************/

void FStudioGameCoreStyle::SetStyle(const TSharedRef< ISlateStyle >& NewStyle)
{
	if (Instance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Instance.Get());
	}

	Instance = NewStyle;

	if (Instance.IsValid())
	{
		FSlateStyleRegistry::RegisterSlateStyle(*Instance.Get());
	}
	else
	{
		ResetToDefault();
	}
}
