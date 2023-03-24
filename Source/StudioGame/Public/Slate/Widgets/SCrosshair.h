// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "Styling/StudioGameSlateTypes.h"
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"


/**
 * SCrosshair
 */
class STUDIOGAME_API SCrosshair : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SCrosshair)
		: _Content()
		, _CrosshairStyle(&FCrosshairStyle::GetDefault())
		, _ColorAndOpacity(FLinearColor::White)
	{}
		/** Slot for this crosshair's content (optional) */
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_STYLE_ARGUMENT(FCrosshairStyle, CrosshairStyle)
		SLATE_ATTRIBUTE(float, Spread)
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/**
	 * Overwritten from SWidget.
	 *
	 * LeafWidgets provide a visual representation of themselves. They do so by adding DrawElement(s)
	 * to the OutDrawElements. DrawElements should have their positions set to absolute coordinates in
	 * Window space; for this purpose the Slate system provides the AllottedGeometry parameter.
	 * AllottedGeometry describes the space allocated for the visualization of this widget.
	 *
	 * Whenever possible, LeafWidgets should avoid dealing with layout properties. See TextBlock for an example.
	 */
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/**
	 * Compute the ideal size necessary to display this widget. For aggregate widgets (e.g. panels) this size should include the
	 * size necessary to show all of its children. CacheDesiredSize() guarantees that the size of descendants is computed and cached
	 * before that of the parents, so it is safe to call GetDesiredSize() for any children while implementing this method.
	 *
	 * Note that ComputeDesiredSize() is meant as an aide to the developer. It is NOT meant to be very robust in many cases. If your
	 * widget is simulating a bouncing ball, you should just return a reasonable size; e.g. 160x160. Let the programmer set up a reasonable
	 * rule of resizing the bouncy ball simulation.
	 *
	 * @param  LayoutScaleMultiplier    This parameter is safe to ignore for almost all widgets; only really affects text measuring.
	 *
	 * @return The desired size.
	 */
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	/** See CrosshairStyle attribute */
	void SetCrosshairStyle(const FCrosshairStyle* JoystickStyle);

	/** Sets the color and opacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/** See Spread attribute */
	void SetSpread(const float& InSpread);

protected:
	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Style resource for the button */
	const FCrosshairStyle* Style;

	/** The value for draw the crosshair content */
	TAttribute<float> Spread;

	/** The slate brushs to draw for the image, or a bound delegate to a brush. */
	const TMap<ECrosshairDirection, FSlateBrush>* CrosshairBrushs;

private:
	/** True if the crosshair should be visible */
	uint32 bVisible : 1;
};
