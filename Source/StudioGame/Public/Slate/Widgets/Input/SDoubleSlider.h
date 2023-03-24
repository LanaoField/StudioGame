// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Styling/SlateWidgetStyleAsset.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Styling/StudioGameSlateTypes.h"
#include "StudioGameTypes.h"
#include "Framework/SlateDelegates.h"
#include "Widgets/SLeafWidget.h"
#include "Animation/InputScaleBias.h"

class FPaintArgs;
class FSlateWindowElementList;

/** Notification for FInputRange value change */
DECLARE_DELEGATE_OneParam(FOnInputRangeValueChanged, FInputRange);

bool operator==(const FInputRange& L, const FInputRange& R)
{
	if (L.Min == R.Min && L.Max == R.Max)
	{
		return true;
	}
	return false;
}

bool operator!=(const FInputRange& L, const FInputRange& R)
{
	if (!(L == R))
	{
		return true;
	}
	return false;
}

/**
 * A Slate slider control is a linear scale and draggable handle.
 */
class STUDIOGAME_API SDoubleSlider : public SLeafWidget
{
#if ENGINE_MAJOR_VERSION >= 5
	SLATE_DECLARE_WIDGET(SDoubleSlider, SLeafWidget)
#endif

public:

	SLATE_BEGIN_ARGS(SDoubleSlider)
		: _IndentHandle(true)
		, _MouseUsesStep(false)
		, _RequiresControllerLock(true)
		, _Locked(false)
		, _Orientation(EOrientation::Orient_Horizontal)
		, _SliderBarColor(FLinearColor::White)
		, _SliderHandleColor(FLinearColor::White)
		, _Style(&FStudioGameCoreStyle::Get().GetWidgetStyle<FDoubleSliderStyle>("DoubleSlider"))
		, _StepSize(0.01f)
		, _Value(FInputRange(0.f, 1.f))
		, _DValue(0.f)
		, _MinValue(0.0f)
		, _MaxValue(1.0f)
		, _IsFocusable(true)
		, _OnMouseCaptureBegin()
		, _OnMouseCaptureEnd()
		, _OnValueChanged()
		{
		}

		/** Whether the slidable area should be indented to fit the handle. */
		SLATE_ATTRIBUTE( bool, IndentHandle )

		/** Sets new value if mouse position is greater/less than half the step size. */
		SLATE_ARGUMENT( bool, MouseUsesStep )

		/** Sets whether we have to lock input to change the slider value. */
		SLATE_ARGUMENT( bool, RequiresControllerLock )

		/** Whether the handle is interactive or fixed. */
		SLATE_ATTRIBUTE( bool, Locked )

		/** The slider's orientation. */
		SLATE_ARGUMENT( EOrientation, Orientation)

		/** The color to draw the slider bar in. */
		SLATE_ATTRIBUTE( FSlateColor, SliderBarColor )

		/** The color to draw the slider handle in. */
		SLATE_ATTRIBUTE( FSlateColor, SliderHandleColor )

		/** The style used to draw the slider. */
		SLATE_STYLE_ARGUMENT( FDoubleSliderStyle, Style )

		/** The input mode while using the controller. */
		SLATE_ATTRIBUTE(float, StepSize)

		/** A value that drives where the slider handle appears. Value is normalized between 0 and 1. */
		SLATE_ATTRIBUTE(FInputRange, Value)

		/** The D-value that can be specified by using the slider. */
		SLATE_ARGUMENT(float, DValue)
		/** The minimum value that can be specified by using the slider. */
		SLATE_ARGUMENT(float, MinValue)
		/** The maximum value that can be specified by using the slider. */
		SLATE_ARGUMENT(float, MaxValue)

		/** Sometimes a slider should only be mouse-clickable and never keyboard focusable. */
		SLATE_ARGUMENT(bool, IsFocusable)

		/** Invoked when the mouse is pressed and a capture begins. */
		SLATE_EVENT(FSimpleDelegate, OnMouseCaptureBegin)

		/** Invoked when the mouse is released and a capture ends. */
		SLATE_EVENT(FSimpleDelegate, OnMouseCaptureEnd)

		/** Invoked when the Controller is pressed and capture begins. */
		SLATE_EVENT(FSimpleDelegate, OnControllerCaptureBegin)

		/** Invoked when the controller capture is released. */
		SLATE_EVENT(FSimpleDelegate, OnControllerCaptureEnd)

		/** Called when the value is changed by the slider. */
		SLATE_EVENT(FOnInputRangeValueChanged, OnValueChanged)

	SLATE_END_ARGS()

	SDoubleSlider();

	/**
	 * Construct the widget.
	 * 
	 * @param InDeclaration A declaration from which to construct the widget.
	 */
	void Construct( const SDoubleSlider::FArguments& InDeclaration );

	/** Set the widget style. */
	void SetStyle(const FDoubleSliderStyle* InStyle);

	/** Get the D-Value attribute */
	float GetDValue() const { return DValue; }

	/** Get the MinValue attribute */
	float GetMinValue() const { return MinValue; }

	/** Get the MaxValue attribute */
	float GetMaxValue() const { return MaxValue; }

	/** Get the Value attribute */
	FInputRange GetValue() const;

	/** Get the Value attribute scaled from 0 to 1 */
	FInputRange GetNormalizedValue() const;

	/** Set the Value attribute */
	void SetValue(TAttribute<FInputRange> InValueAttribute);

	/** Set the DValue attributes. */
	void SetDValue(float InDValue);

	/** Set the MinValue and MaxValue attributes. If the new MinValue is more than the new MaxValue, MaxValue will be changed to equal MinValue. */
	void SetMinAndMaxValues(float InMinValue, float InMaxValue);
	
	/** Set the IndentHandle attribute */
	void SetIndentHandle(TAttribute<bool> InIndentHandle);
	
	/** Set the Locked attribute */
	void SetLocked(TAttribute<bool> InLocked);

	/** Set the Orientation attribute */
	void SetOrientation(EOrientation InOrientation);
	
	/** Set the SliderBarColor attribute */
	void SetSliderBarColor(TAttribute<FSlateColor> InSliderBarColor);
	
	/** Set the SliderHandleColor attribute */
	void SetSliderHandleColor(TAttribute<FSlateColor> InSliderHandleColor);

	/** Get the StepSize attribute */
	float GetStepSize() const;

	/** Set the StepSize attribute */
	void SetStepSize(TAttribute<float> InStepSize);

	/** Set the MouseUsesStep attribute */
	void SetMouseUsesStep(bool MouseUsesStep);

	/** Set the RequiresControllerLock attribute */
	void SetRequiresControllerLock(bool RequiresControllerLock);

public:

	// SWidget overrides

	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	virtual bool SupportsKeyboardFocus() const override;
	virtual bool IsInteractable() const override;
#if WITH_ACCESSIBILITY
	virtual TSharedRef<FSlateAccessibleWidget> CreateAccessibleWidget() override;
#endif

	/** @return Is the handle locked or not? Defaults to false */
	bool IsLocked() const;

protected:

	/**
	 * Commits the specified slider value.
	 *
	 * @param NewValue The value to commit.
	 */
	virtual void CommitValue(FInputRange NewValue);

	/**
	 * Calculates the new value based on the given absolute coordinates.
	 *
	 * @param MyGeometry The slider's geometry.
	 * @param AbsolutePosition The absolute position of the slider.
	 * @return The new value.
	 */
	FInputRange PositionToValue( const FGeometry& MyGeometry, const FVector2D& AbsolutePosition );

	const FSlateBrush* GetBarImage() const;
	const FSlateBrush* GetThumbImage() const;

protected:
#if ENGINE_MAJOR_VERSION >= 5
	/** @return an attribute reference of IndentHandle */
	TSlateAttributeRef<FInputRange> GetValueAttribute() const
	{
		return TSlateAttributeRef<FInputRange>(SharedThis(this), ValueSlateAttribute);
	}

	/** @return an attribute reference of IndentHandle */
	TSlateAttributeRef<bool> GetIndentHandleAttribute() const
	{
		return TSlateAttributeRef<bool>(SharedThis(this), IndentHandleSlateAttribute);
	}

	/** @return an attribute reference of Locked */
	TSlateAttributeRef<bool> GetLockedAttribute() const
	{
		return TSlateAttributeRef<bool>(SharedThis(this), LockedSlateAttribute);
	}

	/** @return an attribute reference of SliderBarColor */
	TSlateAttributeRef<FSlateColor> GetSliderBarColorAttribute() const
	{
		return TSlateAttributeRef<FSlateColor>(SharedThis(this), SliderBarColorSlateAttribute);
	}

	/** @return an attribute reference of SliderHandleColor */
	TSlateAttributeRef<FSlateColor> GetSliderHandleColorAttribute() const
	{
		return TSlateAttributeRef<FSlateColor>(SharedThis(this), SliderHandleColorSlateAttribute);
	}
#else
	/** @return an attribute reference of IndentHandle */
	TAttribute<FInputRange> GetValueAttribute() const
	{
		return ValueSlateAttribute;
	}

	/** @return an attribute reference of IndentHandle */
	TAttribute<bool> GetIndentHandleAttribute() const
	{
		return IndentHandleSlateAttribute;
	}

	/** @return an attribute reference of Locked */
	TAttribute<bool> GetLockedAttribute() const
	{
		return LockedSlateAttribute;
	}

	/** @return an attribute reference of SliderBarColor */
	TAttribute<FSlateColor> GetSliderBarColorAttribute() const
	{
		return SliderBarColorSlateAttribute;
	}

	/** @return an attribute reference of SliderHandleColor */
	TAttribute<FSlateColor> GetSliderHandleColorAttribute() const
	{
		return SliderHandleColorSlateAttribute;
	}
#endif

	// Holds the style passed to the widget upon construction.
	const FDoubleSliderStyle* Style;

	// Holds the slider's orientation.
	EOrientation Orientation;

	// Holds the initial cursor in case a custom cursor has been specified, so we can restore it after dragging the slider
	EMouseCursor::Type CachedCursor;

	/** The location in screenspace the slider was pressed by a touch */
	FVector2D PressedScreenSpaceTouchDownPosition;

	/** Holds the amount to adjust the value by when using a controller or keyboard */
	TAttribute<float> StepSize;

	float DValue;
	float MinValue;
	float MaxValue;

	// Holds a flag indicating whether a controller/keyboard is manipulating the slider's value. 
	// When true, navigation away from the widget is prevented until a new value has been accepted or canceled. 
	bool bControllerInputCaptured;

	/** Sets new value if mouse position is greater/less than half the step size. */
	bool bMouseUsesStep;

	/** Sets whether we have to lock input to change the slider value. */
	bool bRequiresControllerLock;

	/** When true, this slider will be keyboard focusable. Defaults to false. */
	bool bIsFocusable;

private:

	// Resets controller input state. Fires delegates.
	void ResetControllerState();

#if ENGINE_MAJOR_VERSION >= 5
	// Holds the slider's current value.
	TSlateAttribute<FInputRange> ValueSlateAttribute;

	// Holds a flag indicating whether the slideable area should be indented to fit the handle.
	TSlateAttribute<bool> IndentHandleSlateAttribute;

	// Holds a flag indicating whether the slider is locked.
	TSlateAttribute<bool> LockedSlateAttribute;

	// Holds the color of the slider bar.
	TSlateAttribute<FSlateColor> SliderBarColorSlateAttribute;

	// Holds the color of the slider handle.
	TSlateAttribute<FSlateColor> SliderHandleColorSlateAttribute;
#else
	// Holds the slider's current value.
	TAttribute<FInputRange> ValueSlateAttribute;

	// Holds a flag indicating whether the slideable area should be indented to fit the handle.
	TAttribute<bool> IndentHandleSlateAttribute;

	// Holds a flag indicating whether the slider is locked.
	TAttribute<bool> LockedSlateAttribute;

	// Holds the color of the slider bar.
	TAttribute<FSlateColor> SliderBarColorSlateAttribute;

	// Holds the color of the slider handle.
	TAttribute<FSlateColor> SliderHandleColorSlateAttribute;
#endif

	// Holds a delegate that is executed when the mouse is pressed and a capture begins.
	FSimpleDelegate OnMouseCaptureBegin;

	// Holds a delegate that is executed when the mouse is let up and a capture ends.
	FSimpleDelegate OnMouseCaptureEnd;

	// Holds a delegate that is executed when capture begins for controller or keyboard.
	FSimpleDelegate OnControllerCaptureBegin;

	// Holds a delegate that is executed when capture ends for controller or keyboard.
	FSimpleDelegate OnControllerCaptureEnd;

	// Holds a delegate that is executed when the slider's value changed.
	FOnInputRangeValueChanged OnValueChanged;
};
