// Copyright 2022 CQUnreal. All Rights Reserved.

#pragma once

#include "Styling/StudioGameSlateTypes.h"
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Input/Reply.h"
#include "Widgets/SLeafWidget.h"


class STUDIOGAME_API SJoystick : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SJoystick)
		: _Content()
		, _JoystickStyle(&FJoystickStyle::GetDefault())
		, _ColorAndOpacity(FLinearColor::White)
		, _InputMode(EJoystickInputMode::Joystick)
	{}
		/** Slot for this joystick's content (optional) */
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_STYLE_ARGUMENT(FJoystickStyle, JoystickStyle)
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
		SLATE_ARGUMENT(FKey, ActionKey)
		SLATE_ARGUMENT(FKey, XAxis)
		SLATE_ARGUMENT(FKey, YAxis)
		SLATE_ARGUMENT(EJoystickInputMode, InputMode)
		SLATE_ARGUMENT(bool, bFixed)
		SLATE_ARGUMENT(float, XAxisRate)
		SLATE_ARGUMENT(float, YAxisRate)
		SLATE_ARGUMENT(APlayerController*, PCOwner)
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
	 * The system calls this method to notify the widget that a mouse button was pressed within it. This event is bubbled.
	 *
	 * @param MyGeometry The Geometry of the widget receiving the event
	 * @param MouseEvent Information about the input event
	 * @return Whether the event was handled along with possible requests for the system to take action.
	 */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * The system calls this method to notify the widget that a mouse button was release within it. This event is bubbled.
	 *
	 * @param MyGeometry The Geometry of the widget receiving the event
	 * @param MouseEvent Information about the input event
	 * @return Whether the event was handled along with possible requests for the system to take action.
	 */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * The system calls this method to notify the widget that a mouse moved within it. This event is bubbled.
	 *
	 * @param MyGeometry The Geometry of the widget receiving the event
	 * @param MouseEvent Information about the input event
	 * @return Whether the event was handled along with possible requests for the system to take action.
	 */
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/**
	 * Ticks this widget with Geometry.  Override in derived classes, but always call the parent implementation.
	 *
	 * @param  AllottedGeometry The space allotted for this widget
	 * @param  InCurrentTime  Current absolute real time
	 * @param  InDeltaTime  Real time passed since last tick
	 */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

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

	/** See JoystickStyle attribute */
	void SetJoystickStyle(const FJoystickStyle* JoystickStyle);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	void SetXAxisRate(const float InValue);

	void SetYAxisRate(const float InValue);

	void SetFixed(const bool InValue);

	FVector2D GetMovedOffset() const;

protected:
	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Style resource for the button */
	const FJoystickStyle* Style;

	/** The slate brush to draw for the image, or a bound delegate to a brush. */
	const FSlateBrush* VisualImage;

	/** The slate brush to draw for the image, or a bound delegate to a brush. */
	const FSlateBrush* ThumbImage;

	/** The draw center point of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute) */
	FVector2D DrawCenter;

	/** The center point of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute) */
	FVector2D Center;

	/** The size of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute) */
	FVector2D VisualSize;

	/** For sticks, the size of the thumb (if <= 1.0, it's relative to screen, > 1.0 is absolute) */
	FVector2D ThumbSize;

	/// Keys
	FKey ActionKey;
	FKey XAxis;
	FKey YAxis;

	/// Rate
	float XAxisRate;
	float YAxisRate;

	/// InputMode
	EJoystickInputMode InputMode;

	/// PCOwner
	APlayerController* PCOwner;

	// Get Gamepad Axis
	const FGamepadKeyNames::Type GetGamepadXAxis() const;
	const FGamepadKeyNames::Type GetGamepadYAxis() const;

private:
	/** True if the joystick should be visible */
	uint32 bVisible : 1;

	/** True if the joystick should be fixed */
	uint32 bFixed : 1;

	/** current button pointer index */
	int32 PointerIndex;

	/** last joystick moved Postion */
	FVector2D StartedPosition;

	/** joystick button moved Postion */
	FVector2D PositionOffset;

	/** last joystick moved offset */
	FVector2D MovedOffset;

	/** last joystick last moved offset */
	FVector2D LastMovedOffset;
	
	/** The position of the thumb, in relation to the visual center */
	FVector2D ThumbPosition;

	/** Whether the current mouse is pressed */
	bool bMouseButtonDown;
};
