// Copyright 2023 CQUnreal. All Rights Reserved.

#pragma once

#include "Widgets\Layout\SScrollBox.h"

DECLARE_DELEGATE_OneParam(FOnScrollEnd, float);

class STUDIOGAME_API SScrollBoxEx : public SScrollBox
{
#if ENGINE_MAJOR_VERSION >= 5
public:

	SLATE_BEGIN_ARGS(SScrollBoxEx)
		: _Style(&FAppStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox"))
		, _ScrollBarStyle(&FAppStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar"))
		, _ExternalScrollbar()
		, _Orientation(Orient_Vertical)
		, _ScrollBarVisibility(EVisibility::Visible)
		, _ScrollBarAlwaysVisible(false)
		, _ScrollBarDragFocusCause(EFocusCause::Mouse)
		, _ScrollBarThickness(FVector2D(_Style->BarThickness, _Style->BarThickness))
		, _ScrollBarPadding(2.0f)
		, _AllowOverscroll(EAllowOverscroll::Yes)
		, _BackPadScrolling(false)
		, _FrontPadScrolling(false)
		, _AnimateWheelScrolling(false)
		, _WheelScrollMultiplier(1.f)
		, _NavigationDestination(EDescendantScrollDestination::IntoView)
		, _NavigationScrollPadding(0.0f)
		, _ScrollWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
		, _OnUserScrolled()
		, _OnScrollEnd()
		, _ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	{
		_Clipping = EWidgetClipping::ClipToBounds;
	}

		SLATE_SLOT_ARGUMENT(FSlot, Slots)

		/** Style used to draw this scrollbox */
		SLATE_STYLE_ARGUMENT(FScrollBoxStyle, Style)

		/** Style used to draw this scrollbox's scrollbar */
		SLATE_STYLE_ARGUMENT(FScrollBarStyle, ScrollBarStyle)

		/** Custom scroll bar */
		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)

		/** The direction that children will be stacked, and also the direction the box will scroll. */
		SLATE_ARGUMENT(EOrientation, Orientation)

		SLATE_ARGUMENT(EVisibility, ScrollBarVisibility)

		SLATE_ARGUMENT(bool, ScrollBarAlwaysVisible)

		SLATE_ARGUMENT(EFocusCause, ScrollBarDragFocusCause)

		SLATE_ARGUMENT(FVector2D, ScrollBarThickness)

		/** This accounts for total internal scroll bar padding; default 2.0f padding from the scroll bar itself is removed */
		SLATE_ARGUMENT(FMargin, ScrollBarPadding)

		SLATE_ARGUMENT(EAllowOverscroll, AllowOverscroll);

		SLATE_ARGUMENT(bool, BackPadScrolling);

		SLATE_ARGUMENT(bool, FrontPadScrolling);

		SLATE_ARGUMENT(bool, AnimateWheelScrolling);

		SLATE_ARGUMENT(float, WheelScrollMultiplier);

		SLATE_ARGUMENT(EDescendantScrollDestination, NavigationDestination);

		/**
		 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
		 * scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
		 */
		SLATE_ARGUMENT(float, NavigationScrollPadding);

		SLATE_ARGUMENT(EScrollWhenFocusChanges, ScrollWhenFocusChanges);

		/** Called when the button is clicked */
		SLATE_EVENT(FOnUserScrolled, OnUserScrolled)

		SLATE_EVENT(FOnScrollEnd, OnScrollEnd)

		SLATE_ARGUMENT(EConsumeMouseWheel, ConsumeMouseWheel);

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void EndInertialScrolling();

	/** Sets the current orientation of the scrollbox and updates the layout */
	void SetOrientation(EOrientation InOrientation);

	/** Adds a slot to SScrollBox */
	FScopedWidgetSlotArguments AddSlot();

	/** Removes a slot at the specified location */
	void RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove);

	/** Removes all children from the box */
	void ClearChildren();

	float GetViewFraction() const;

	float GetViewOffsetFraction() const;

	/**
	 * Attempt to scroll a widget into view, will safely handle non-descendant widgets
	 *
	 * @param WidgetToFind The widget whose geometry we wish to discover.
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @param InDestination		Where we want the child widget to stop.
	 */
	void ScrollDescendantIntoView(const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	/** Gets the scroll offset of the bottom of the ScrollBox in Slate Units. */
	float GetScrollOffsetOfEnd() const;

	float GetScrollOffset() const;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;

	// SWidget interface
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	// End of SWidget interface

protected:

	FOnScrollEnd OnScrollEnd;

	/** The panel which stacks the child slots */
	TSharedPtr<class SScrollPanelEx> ScrollPanel;

	/** Scrolls or begins scrolling a widget into view, only valid to call when we have layout geometry. */
	bool InternalScrollDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	void OnClippingChanged();

private:
	float DesiredScrollOffset;

	/** Padding to the scrollbox */
	FMargin ScrollBarSlotPadding;

	union
	{
		// vertical scroll bar is stored in horizontal box and vice versa
		SHorizontalBox::FSlot* VerticalScrollBarSlot; // valid when Orientation == Orient_Vertical
		SVerticalBox::FSlot* HorizontalScrollBarSlot; // valid when Orientation == Orient_Horizontal
	};

private:

	/** Builds a default Scrollbar */
	TSharedPtr<SScrollBar> ConstructScrollBar();

	/** Invoked when the user scroll via the scrollbar */
	void ScrollBar_OnUserScrolled(float InScrollOffsetFraction);

	/** Gets the component of a vector in the direction of scrolling based on the Orientation property. */
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2
	FORCEINLINE float GetScrollComponentFromVector(FVector2f Vector) const
#else
	FORCEINLINE float GetScrollComponentFromVector(FVector2D Vector) const
#endif
	{
		return float(Orientation == Orient_Vertical ? Vector.Y : Vector.X);
	}

	/** Sets the component of a vector in the direction of scrolling based on the Orientation property. */
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2
	inline void SetScrollComponentOnVector(FVector2f& InVector, float Value) const
#else
	inline void SetScrollComponentOnVector(FVector2D& InVector, float Value) const
#endif
	{
		if (Orientation == Orient_Vertical)
		{
			InVector.Y = Value;
		}
		else
		{
			InVector.X = Value;
		}
	}

	/** Constructs internal layout widgets for scrolling vertically using the existing ScrollPanel and ScrollBar. */
	void ConstructVerticalLayout();

	/** Constructs internal layout widgets for scrolling horizontally using the existing ScrollPanel and ScrollBar. */
	void ConstructHorizontalLayout();

	/** Does the user need a hint that they can scroll to the start of the list? */
	FSlateColor GetStartShadowOpacity() const;

	/** Does the user need a hint that they can scroll to the end of the list? */
	FSlateColor GetEndShadowOpacity() const;

	/** Check whether the current state of the table warrants inertial scroll by the specified amount */
	bool CanUseInertialScroll(float ScrollAmount) const;

	/**
	 * Scroll the view by ScrollAmount given its currently AllottedGeometry.
	 *
	 * @param AllottedGeometry  The geometry allotted for this SScrollBox by the parent
	 * @param ScrollAmount
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @return Whether or not the scroll was fully handled
	 */
	bool ScrollBy(const FGeometry& AllottedGeometry, float LocalScrollAmount, EAllowOverscroll Overscroll, bool InAnimateScroll);

	void BeginInertialScrolling();

	/** Active timer to update inertial scrolling as needed */
	EActiveTimerReturnType UpdateInertialScroll(double InCurrentTime, float InDeltaTime);
};

class SScrollPanelEx : public SPanel
{
public:

	SLATE_BEGIN_ARGS(SScrollPanelEx)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}

	SLATE_ARGUMENT(EOrientation, Orientation)
		SLATE_ARGUMENT(bool, BackPadScrolling)
		SLATE_ARGUMENT(bool, FrontPadScrolling)

		SLATE_END_ARGS()

		SScrollPanelEx()
		: Children(this)
	{
	}

	UE_DEPRECATED(5.0, "Direct construction of FSlot is deprecated")
		void Construct(const FArguments& InArgs, const TArray<SScrollBoxEx::FSlot*>& InSlots);

	void Construct(const FArguments& InArgs, TArray<SScrollBoxEx::FSlot::FSlotArguments> InSlots);

public:

	EOrientation GetOrientation()
	{
		return Orientation;
	}

	void SetOrientation(EOrientation InOrientation)
	{
		Orientation = InOrientation;
	}

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	virtual FChildren* GetChildren() override
	{
		return &Children;
	}

	float PhysicalOffset;
	TPanelChildren<SScrollBoxEx::FSlot> Children;

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

private:

	float ArrangeChildVerticalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBoxEx::FSlot& ThisSlot, float CurChildOffset) const;
	float ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBoxEx::FSlot& ThisSlot, float CurChildOffset) const;

private:

	EOrientation Orientation;
	bool BackPadScrolling;
	bool FrontPadScrolling;
#endif
};