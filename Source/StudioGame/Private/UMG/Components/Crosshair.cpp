// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Components/Crosshair.h"

#define LOCTEXT_NAMESPACE "UMG"

UCrosshair::UCrosshair(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ColorAndOpacity(FLinearColor::White)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UCrosshair::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UCrosshair::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyCrosshair.Reset();
}

void UCrosshair::SetStyle(const FCrosshairStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyCrosshair.IsValid())
	{
		MyCrosshair->SetCrosshairStyle(&WidgetStyle);
	}
}

void UCrosshair::SetSpread(const float& InSpread)
{
	Spread = InSpread;
	if (MyCrosshair.IsValid())
	{
		MyCrosshair->SetSpread(Spread);
	}
}

void UCrosshair::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (MyCrosshair.IsValid())
	{
		MyCrosshair->SetColorAndOpacity(ColorAndOpacity);
	}
}

TSharedRef<SWidget> UCrosshair::RebuildWidget()
{
	MyCrosshair = SNew(SCrosshair)
		.CrosshairStyle(&WidgetStyle)
		.Spread(Spread)
		.ColorAndOpacity(ColorAndOpacity);

	return MyCrosshair.ToSharedRef();
}

#if WITH_EDITOR

const FText UCrosshair::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

