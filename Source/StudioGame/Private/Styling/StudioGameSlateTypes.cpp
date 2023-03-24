// Copyright Epic Games, Inc. All Rights Reserved.

#include "Styling/StudioGameSlateTypes.h"


const FName FDoubleSliderStyle::TypeName(TEXT("FDoubleSliderStyle"));

FDoubleSliderStyle::FDoubleSliderStyle()
	: BarThickness(2.0f)
{
}

void FDoubleSliderStyle::GetResources(TArray< const FSlateBrush* >& OutBrushes) const
{
	OutBrushes.Add(&NormalBarImage);
	OutBrushes.Add(&HoveredBarImage);
	OutBrushes.Add(&DisabledBarImage);
	OutBrushes.Add(&NormalThumbImage);
	OutBrushes.Add(&HoveredThumbImage);
	OutBrushes.Add(&DisabledThumbImage);
}

const FDoubleSliderStyle& FDoubleSliderStyle::GetDefault()
{
	static FDoubleSliderStyle Default;
	return Default;
}

const FName FJoystickStyle::TypeName(TEXT("FJoystickStyle"));

void FJoystickStyle::GetResources(TArray< const FSlateBrush* >& OutBrushes) const
{
	OutBrushes.Add(&VisualBrush);
	OutBrushes.Add(&ThumbBrush);
}

const FJoystickStyle& FJoystickStyle::GetDefault()
{
	static FJoystickStyle Default;
	return Default;
}

const FName FCrosshairStyle::TypeName(TEXT("FCrosshairStyle"));

void FCrosshairStyle::GetResources(TArray< const FSlateBrush* >& OutBrushes) const
{
	for (const TPair<ECrosshairDirection, FSlateBrush>& Pair : CrosshairBrushs)
	{
		OutBrushes.Add(&Pair.Value);
	}
}

const FCrosshairStyle& FCrosshairStyle::GetDefault()
{
	static FCrosshairStyle Default;
	return Default;
}