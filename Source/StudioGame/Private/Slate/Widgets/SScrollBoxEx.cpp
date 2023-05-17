// Copyright 2023 CQUnreal. All Rights Reserved.

#include "Widgets/SScrollBoxEx.h"
#include "Layout/LayoutUtils.h"

#if ENGINE_MAJOR_VERSION >= 5
#define LOCTEXT_NAMESPACE "UMG"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void SScrollPanelEx::Construct(const FArguments& InArgs, const TArray<SScrollBoxEx::FSlot*>& InSlots)
{
	PhysicalOffset = 0;
	Children.Reserve(InSlots.Num());
	for (int32 SlotIndex = 0; SlotIndex < InSlots.Num(); ++SlotIndex)
	{
		Children.Add(InSlots[SlotIndex]);
	}
	Orientation = InArgs._Orientation;
	BackPadScrolling = InArgs._BackPadScrolling;
	FrontPadScrolling = InArgs._FrontPadScrolling;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS


void SScrollPanelEx::Construct(const FArguments& InArgs, TArray<SScrollBoxEx::FSlot::FSlotArguments> InSlots)
{
	PhysicalOffset = 0;
	Children.AddSlots(MoveTemp(InSlots));
	Orientation = InArgs._Orientation;
	BackPadScrolling = InArgs._BackPadScrolling;
	FrontPadScrolling = InArgs._FrontPadScrolling;
}

void SScrollPanelEx::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	float ScrollPadding = Orientation == Orient_Vertical ? AllottedGeometry.GetLocalSize().Y : AllottedGeometry.GetLocalSize().X;
	float CurChildOffset = -PhysicalOffset;
	CurChildOffset += BackPadScrolling ? ScrollPadding : 0;

	for (int32 SlotIndex = 0; SlotIndex < Children.Num(); ++SlotIndex)
	{
		const SScrollBoxEx::FSlot& ThisSlot = Children[SlotIndex];
		const EVisibility ChildVisibility = ThisSlot.GetWidget()->GetVisibility();

		if (ChildVisibility != EVisibility::Collapsed)
		{
			if (Orientation == Orient_Vertical)
			{
				CurChildOffset = ArrangeChildVerticalAndReturnOffset(AllottedGeometry, ArrangedChildren, ThisSlot, CurChildOffset);
			}
			else
			{
				CurChildOffset = ArrangeChildHorizontalAndReturnOffset(AllottedGeometry, ArrangedChildren, ThisSlot, CurChildOffset);
			}
		}
	}
}

FVector2D SScrollPanelEx::ComputeDesiredSize(float) const
{
	FVector2D ThisDesiredSize = FVector2D::ZeroVector;
	for (int32 SlotIndex = 0; SlotIndex < Children.Num(); ++SlotIndex)
	{
		const SScrollBoxEx::FSlot& ThisSlot = Children[SlotIndex];
		if (ThisSlot.GetWidget()->GetVisibility() != EVisibility::Collapsed)
		{
			const FVector2D ChildDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
			if (Orientation == Orient_Vertical)
			{
				ThisDesiredSize.X = FMath::Max(ChildDesiredSize.X, ThisDesiredSize.X);
				ThisDesiredSize.Y += ChildDesiredSize.Y + ThisSlot.GetPadding().GetTotalSpaceAlong<Orient_Vertical>();
			}
			else
			{
				ThisDesiredSize.X += ChildDesiredSize.X + ThisSlot.GetPadding().GetTotalSpaceAlong<Orient_Horizontal>();
				ThisDesiredSize.Y = FMath::Max(ChildDesiredSize.Y, ThisDesiredSize.Y);
			}
		}
	}

	FVector2D::FReal ScrollPadding = Orientation == Orient_Vertical ? GetTickSpaceGeometry().GetLocalSize().Y : GetTickSpaceGeometry().GetLocalSize().X;
	FVector2D::FReal& SizeSideToPad = Orientation == Orient_Vertical ? ThisDesiredSize.Y : ThisDesiredSize.X;
	SizeSideToPad += BackPadScrolling ? ScrollPadding : 0;
	SizeSideToPad += FrontPadScrolling ? ScrollPadding : 0;

	return ThisDesiredSize;
}

float SScrollPanelEx::ArrangeChildVerticalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBoxEx::FSlot& ThisSlot, float CurChildOffset) const
{
	const FMargin& ThisPadding = ThisSlot.GetPadding();
	const FVector2D& WidgetDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
	const float ThisSlotDesiredHeight = WidgetDesiredSize.Y + ThisPadding.GetTotalSpaceAlong<Orient_Vertical>();

	// Figure out the size and local position of the child within the slot.  There is no vertical alignment, because 
	// it does not make sense in a panel where items are stacked vertically end-to-end.
	AlignmentArrangeResult XAlignmentResult = AlignChild<Orient_Horizontal>(AllottedGeometry.GetLocalSize().X, ThisSlot, ThisPadding);

	ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(ThisSlot.GetWidget(), FVector2D(XAlignmentResult.Offset, CurChildOffset + ThisPadding.Top), FVector2D(XAlignmentResult.Size, WidgetDesiredSize.Y)));
	return CurChildOffset + ThisSlotDesiredHeight;
}

float SScrollPanelEx::ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBoxEx::FSlot& ThisSlot, float CurChildOffset) const
{
	const FMargin& ThisPadding = ThisSlot.GetPadding();
	const FVector2D& WidgetDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
	const float ThisSlotDesiredWidth = WidgetDesiredSize.X + ThisPadding.GetTotalSpaceAlong<Orient_Horizontal>();

	// Figure out the size and local position of the child within the slot.  There is no horizontal alignment, because
	// it doesn't make sense in a panel where items are stacked horizontally end-to-end.
	AlignmentArrangeResult YAlignmentResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, ThisSlot, ThisPadding);

	ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(ThisSlot.GetWidget(), FVector2D(CurChildOffset + ThisPadding.Left, YAlignmentResult.Offset), FVector2D(WidgetDesiredSize.X, YAlignmentResult.Size)));
	return CurChildOffset + ThisSlotDesiredWidth;
}


void SScrollBoxEx::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);

	Style = InArgs._Style;
	ScrollBarStyle = InArgs._ScrollBarStyle;
	DesiredScrollOffset = 0;
	bIsScrolling = false;
	bAnimateScroll = false;
	AmountScrolledWhileRightMouseDown = 0;
	PendingScrollTriggerAmount = 0;
	bShowSoftwareCursor = false;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2
	SoftwareCursorPosition = FVector2f::ZeroVector;
#else
	SoftwareCursorPosition = FVector2D::ZeroVector;
#endif
	OnUserScrolled = InArgs._OnUserScrolled;
	OnScrollEnd = InArgs._OnScrollEnd;
	Orientation = InArgs._Orientation;
	bScrollToEnd = false;
	bIsScrollingActiveTimerRegistered = false;
	bAllowsRightClickDragScrolling = false;
	ConsumeMouseWheel = InArgs._ConsumeMouseWheel;
	TickScrollDelta = 0;
	AllowOverscroll = InArgs._AllowOverscroll;
	BackPadScrolling = InArgs._BackPadScrolling;
	FrontPadScrolling = InArgs._FrontPadScrolling;
	bAnimateWheelScrolling = InArgs._AnimateWheelScrolling;
	WheelScrollMultiplier = InArgs._WheelScrollMultiplier;
	NavigationScrollPadding = InArgs._NavigationScrollPadding;
	NavigationDestination = InArgs._NavigationDestination;
	ScrollWhenFocusChanges = InArgs._ScrollWhenFocusChanges;
	bTouchPanningCapture = false;
	bVolatilityAlwaysInvalidatesPrepass = true;

	if (InArgs._ExternalScrollbar.IsValid())
	{
		// An external scroll bar was specified by the user
		ScrollBar = InArgs._ExternalScrollbar;
		ScrollBar->SetOnUserScrolled(FOnUserScrolled::CreateSP(this, &SScrollBoxEx::ScrollBar_OnUserScrolled));
		bScrollBarIsExternal = true;
	}
	else
	{
		// Make a scroll bar 
		ScrollBar = ConstructScrollBar();
		ScrollBar->SetDragFocusCause(InArgs._ScrollBarDragFocusCause);
		ScrollBar->SetThickness(InArgs._ScrollBarThickness);
		ScrollBar->SetUserVisibility(InArgs._ScrollBarVisibility);
		ScrollBar->SetScrollBarAlwaysVisible(InArgs._ScrollBarAlwaysVisible);
		ScrollBarSlotPadding = InArgs._ScrollBarPadding;

		bScrollBarIsExternal = false;
	}

	SAssignNew(ScrollPanel, SScrollPanelEx, MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots)))
		.Clipping(InArgs._Clipping)
		.Orientation(Orientation)
		.BackPadScrolling(BackPadScrolling)
		.FrontPadScrolling(FrontPadScrolling);

	if (Orientation == Orient_Vertical)
	{
		ConstructVerticalLayout();
	}
	else
	{
		ConstructHorizontalLayout();
	}

	ScrollBar->SetState(0.0f, 1.0f);
}

bool SScrollBoxEx::InternalScrollDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll, EDescendantScrollDestination InDestination, float InScrollPadding)
{
	// We need to safely find the one WidgetToFind among our descendants.
	TSet< TSharedRef<SWidget> > WidgetsToFind;
	{
		if (WidgetToFind.IsValid())
		{
			WidgetsToFind.Add(WidgetToFind.ToSharedRef());
		}
	}
	TMap<TSharedRef<SWidget>, FArrangedWidget> Result;

	FindChildGeometries(MyGeometry, WidgetsToFind, Result);

	if (WidgetToFind.IsValid())
	{
		FArrangedWidget* WidgetGeometry = Result.Find(WidgetToFind.ToSharedRef());
		if (!WidgetGeometry)
		{
			UE_LOG(LogSlate, Warning, TEXT("Unable to scroll to descendant as it's not a child of the scrollbox"));
		}
		else
		{
			float ScrollOffset = 0.0f;
			if (InDestination == EDescendantScrollDestination::TopOrLeft)
			{
				// Calculate how much we would need to scroll to bring this to the top/left of the scroll box
				const float WidgetPosition = GetScrollComponentFromVector(MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()));
				const float MyPosition = InScrollPadding;
				ScrollOffset = WidgetPosition - MyPosition;
			}
			else if (InDestination == EDescendantScrollDestination::BottomOrRight)
			{
				// Calculate how much we would need to scroll to bring this to the bottom/right of the scroll box
				const float WidgetPosition = GetScrollComponentFromVector(MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition() + WidgetGeometry->Geometry.GetAbsoluteSize()) - MyGeometry.GetLocalSize());
				const float MyPosition = InScrollPadding;
				ScrollOffset = WidgetPosition - MyPosition;
			}
			else if (InDestination == EDescendantScrollDestination::Center)
			{
				// Calculate how much we would need to scroll to bring this to the top/left of the scroll box
				const float WidgetPosition = GetScrollComponentFromVector(MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()) + (WidgetGeometry->Geometry.GetLocalSize() / 2));
				const float MyPosition = GetScrollComponentFromVector(MyGeometry.GetLocalSize() * FVector2D(0.5f, 0.5f));
				ScrollOffset = WidgetPosition - MyPosition;
			}
			else
			{
				const float WidgetStartPosition = GetScrollComponentFromVector(MyGeometry.AbsoluteToLocal(WidgetGeometry->Geometry.GetAbsolutePosition()));
				const float WidgetEndPosition = WidgetStartPosition + GetScrollComponentFromVector(WidgetGeometry->Geometry.GetLocalSize());
				const float ViewStartPosition = InScrollPadding;
				const float ViewEndPosition = GetScrollComponentFromVector(MyGeometry.GetLocalSize() - InScrollPadding);

				const float ViewDelta = (ViewEndPosition - ViewStartPosition);
				const float WidgetDelta = (WidgetEndPosition - WidgetStartPosition);

				if (WidgetStartPosition < ViewStartPosition)
				{
					ScrollOffset = WidgetStartPosition - ViewStartPosition;
				}
				else if (WidgetEndPosition > ViewEndPosition)
				{
					ScrollOffset = (WidgetEndPosition - ViewDelta) - ViewStartPosition;
				}
			}

			if (ScrollOffset != 0.0f)
			{
				DesiredScrollOffset = ScrollPanel->PhysicalOffset;
				ScrollBy(MyGeometry, ScrollOffset, EAllowOverscroll::No, InAnimateScroll);
			}

			return true;
		}
	}

	return false;
}

void SScrollBoxEx::OnClippingChanged()
{
	ScrollPanel->SetClipping(Clipping);
}

TSharedPtr<SScrollBar> SScrollBoxEx::ConstructScrollBar()
{
	return TSharedPtr<SScrollBar>(SNew(SScrollBar)
		.Style(ScrollBarStyle)
		.Orientation(Orientation)
		.Padding(0.0f)
		.OnUserScrolled(this, &SScrollBoxEx::ScrollBar_OnUserScrolled));
}

void SScrollBoxEx::ScrollBar_OnUserScrolled(float InScrollOffsetFraction)
{
	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());
	const FGeometry ScrollPanelGeometry = FindChildGeometry(CachedGeometry, ScrollPanel.ToSharedRef());

	// Clamp to max scroll offset
	DesiredScrollOffset = FMath::Min(InScrollOffsetFraction * ContentSize, ContentSize - GetScrollComponentFromVector(ScrollPanelGeometry.GetLocalSize()));
	OnUserScrolled.ExecuteIfBound(DesiredScrollOffset);

	Invalidate(EInvalidateWidget::Layout);
}

void SScrollBoxEx::ConstructVerticalLayout()
{
	TSharedPtr<SHorizontalBox> PanelAndScrollbar;
	this->ChildSlot
		[
			SAssignNew(PanelAndScrollbar, SHorizontalBox)

			+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
		.Padding(FMargin(0.0f, 0.0f, 0.0f, 1.0f))
		[
			// Scroll panel that presents the scrolled content
			ScrollPanel.ToSharedRef()
		]

	+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			// Shadow: Hint to scroll up
			SNew(SImage)
			.Visibility(EVisibility::HitTestInvisible)
		.ColorAndOpacity(this, &SScrollBoxEx::GetStartShadowOpacity)
		.Image(&Style->TopShadowBrush)
		]

	+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			// Shadow: a hint to scroll down
			SNew(SImage)
			.Visibility(EVisibility::HitTestInvisible)
		.ColorAndOpacity(this, &SScrollBoxEx::GetEndShadowOpacity)
		.Image(&Style->BottomShadowBrush)
		]
		]
		];

	VerticalScrollBarSlot = nullptr;
	if (!bScrollBarIsExternal)
	{
		PanelAndScrollbar->AddSlot()
			.Padding(ScrollBarSlotPadding)
			.AutoWidth()
			.Expose(VerticalScrollBarSlot)
			[
				ScrollBar.ToSharedRef()
			];
	}
}

void SScrollBoxEx::ConstructHorizontalLayout()
{
	TSharedPtr<SVerticalBox> PanelAndScrollbar;
	this->ChildSlot
		[
			SAssignNew(PanelAndScrollbar, SVerticalBox)

			+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
		.Padding(FMargin(0.0f, 0.0f, 1.0f, 0.0f))
		[
			// Scroll panel that presents the scrolled content
			ScrollPanel.ToSharedRef()
		]

	+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Fill)
		[
			// Shadow: Hint to left
			SNew(SImage)
			.Visibility(EVisibility::HitTestInvisible)
		.ColorAndOpacity(this, &SScrollBoxEx::GetStartShadowOpacity)
		.Image(&Style->LeftShadowBrush)
		]

	+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Fill)
		[
			// Shadow: a hint to scroll right
			SNew(SImage)
			.Visibility(EVisibility::HitTestInvisible)
		.ColorAndOpacity(this, &SScrollBoxEx::GetEndShadowOpacity)
		.Image(&Style->RightShadowBrush)
		]
		]
		];

	HorizontalScrollBarSlot = nullptr;
	if (!bScrollBarIsExternal)
	{
		PanelAndScrollbar->AddSlot()
			.Padding(ScrollBarSlotPadding)
			.AutoHeight()
			.Expose(HorizontalScrollBarSlot)
			[
				ScrollBar.ToSharedRef()
			];
	}
}

const float ShadowFadeDistance = 32.0f;

FSlateColor SScrollBoxEx::GetStartShadowOpacity() const
{
	// The shadow should only be visible when the user needs a hint that they can scroll up.
	const float ShadowOpacity = FMath::Clamp(ScrollPanel->PhysicalOffset / ShadowFadeDistance, 0.0f, 1.0f);

	return FLinearColor(1.0f, 1.0f, 1.0f, ShadowOpacity);
}

FSlateColor SScrollBoxEx::GetEndShadowOpacity() const
{
	// The shadow should only be visible when the user needs a hint that they can scroll down.
	const float ShadowOpacity = (ScrollBar->DistanceFromBottom() * GetScrollComponentFromVector(ScrollPanel->GetDesiredSize()) / ShadowFadeDistance);

	return FLinearColor(1.0f, 1.0f, 1.0f, ShadowOpacity);
}

void SScrollBoxEx::EndInertialScrolling()
{
	bIsScrolling = false;
	bIsScrollingActiveTimerRegistered = false;
	Invalidate(EInvalidateWidget::LayoutAndVolatility);
	if (UpdateInertialScrollHandle.IsValid())
	{
		UnRegisterActiveTimer(UpdateInertialScrollHandle.ToSharedRef());
		UpdateInertialScrollHandle.Reset();
	}

	// Zero the scroll velocity so the panel stops immediately on mouse down, even if the user does not drag
	InertialScrollManager.ClearScrollVelocity();
	OnScrollEnd.ExecuteIfBound(DesiredScrollOffset);
}

void SScrollBoxEx::SetOrientation(EOrientation InOrientation)
{
	if (Orientation != InOrientation)
	{
		Orientation = InOrientation;
		if (!bScrollBarIsExternal)
		{
			ScrollBar = ConstructScrollBar();
		}
		ScrollPanel->SetOrientation(Orientation);
		if (Orientation == Orient_Vertical)
		{
			ConstructVerticalLayout();
		}
		else
		{
			ConstructHorizontalLayout();
		}
	}
}

/** Adds a slot to SScrollBox */
SScrollBoxEx::FScopedWidgetSlotArguments SScrollBoxEx::AddSlot()
{
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(), ScrollPanel->Children, INDEX_NONE };
}

/** Removes a slot at the specified location */
void SScrollBoxEx::RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove)
{
	ScrollPanel->Children.Remove(WidgetToRemove);
}

void SScrollBoxEx::ClearChildren()
{
	ScrollPanel->Children.Empty();
}

float SScrollBoxEx::GetScrollOffsetOfEnd() const
{
	const FGeometry ScrollPanelGeometry = FindChildGeometry(CachedGeometry, ScrollPanel.ToSharedRef());
	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());
	return FMath::Max(ContentSize - GetScrollComponentFromVector(ScrollPanelGeometry.Size), 0.0f);
}

float SScrollBoxEx::GetScrollOffset() const
{
	return DesiredScrollOffset;
}

float SScrollBoxEx::GetViewFraction() const
{
	const FGeometry ScrollPanelGeometry = FindChildGeometry(CachedGeometry, ScrollPanel.ToSharedRef());
	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());

	return FMath::Clamp<float>(GetScrollComponentFromVector(CachedGeometry.GetLocalSize()) > 0 ? GetScrollComponentFromVector(ScrollPanelGeometry.Size) / ContentSize : 1, 0.0f, 1.0f);
}

float SScrollBoxEx::GetViewOffsetFraction() const
{
	const FGeometry ScrollPanelGeometry = FindChildGeometry(CachedGeometry, ScrollPanel.ToSharedRef());
	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());

	const float ViewFraction = GetViewFraction();
	return FMath::Clamp(DesiredScrollOffset / ContentSize, 0.f, 1.f - ViewFraction);
}

void SScrollBoxEx::ScrollDescendantIntoView(const TSharedPtr<SWidget>& WidgetToScrollIntoView, bool InAnimateScroll, EDescendantScrollDestination InDestination, float InScrollPadding)
{
	ScrollIntoViewRequest = [this, WidgetToScrollIntoView, InAnimateScroll, InDestination, InScrollPadding](FGeometry AllottedGeometry) {
		InternalScrollDescendantIntoView(AllottedGeometry, WidgetToScrollIntoView, InAnimateScroll, InDestination, InScrollPadding);
	};

	BeginInertialScrolling();
}

void SScrollBoxEx::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	CachedGeometry = AllottedGeometry;

	if (bTouchPanningCapture && (FSlateApplication::Get().GetCurrentTime() - LastScrollTime) > 0.10)
	{
		InertialScrollManager.ClearScrollVelocity();
	}

	// If we needed a widget to be scrolled into view, make that happen.
	if (ScrollIntoViewRequest)
	{
		ScrollIntoViewRequest(AllottedGeometry);
		ScrollIntoViewRequest = nullptr;
	}

	const FGeometry ScrollPanelGeometry = FindChildGeometry(AllottedGeometry, ScrollPanel.ToSharedRef());
	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());

	if (bScrollToEnd)
	{
		DesiredScrollOffset = FMath::Max(ContentSize - GetScrollComponentFromVector(ScrollPanelGeometry.GetLocalSize()), 0.0f);
		bScrollToEnd = false;
	}

	// If this scroll box has no size, do not compute a view fraction because it will be wrong and causes pop in when the size is available
	const float ViewFraction = GetViewFraction();
	const float TargetViewOffset = GetViewOffsetFraction();

	const float CurrentViewOffset = bAnimateScroll ? FMath::FInterpTo(ScrollBar->DistanceFromTop(), TargetViewOffset, InDeltaTime, 15.f) : TargetViewOffset;

	// Update the scrollbar with the clamped version of the offset
	float NewPhysicalOffset = GetScrollComponentFromVector(CurrentViewOffset * ScrollPanel->GetDesiredSize());
	if (AllowOverscroll == EAllowOverscroll::Yes)
	{
		NewPhysicalOffset += Overscroll.GetOverscroll(AllottedGeometry);
	}

	const bool bWasScrolling = bIsScrolling;
	bIsScrolling = !FMath::IsNearlyEqual(NewPhysicalOffset, ScrollPanel->PhysicalOffset, 0.001f);

	ScrollPanel->PhysicalOffset = NewPhysicalOffset;

	if (bWasScrolling && !bIsScrolling)
	{
		Invalidate(EInvalidateWidget::Layout);
	}

	ScrollBar->SetState(CurrentViewOffset, ViewFraction);
	if (!ScrollBar->IsNeeded())
	{
		// We cannot scroll, so ensure that there is no offset.
		ScrollPanel->PhysicalOffset = 0.0f;
	}
}

FNavigationReply SScrollBoxEx::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
	TSharedPtr<SWidget> FocusedChild;
	int32 FocusedChildIndex = -1;
	int32 FocusedChildDirection = 0;

	// Find the child with focus currently so that we can find the next logical child we're going to move to.
	TPanelChildren<SScrollBoxEx::FSlot>& Children = ScrollPanel->Children;
	for (int32 SlotIndex = 0; SlotIndex < Children.Num(); ++SlotIndex)
	{
		if (Children[SlotIndex].GetWidget()->HasUserFocus(InNavigationEvent.GetUserIndex()).IsSet() ||
			Children[SlotIndex].GetWidget()->HasUserFocusedDescendants(InNavigationEvent.GetUserIndex()))
		{
			FocusedChild = Children[SlotIndex].GetWidget();
			FocusedChildIndex = SlotIndex;
			break;
		}
	}

	if (FocusedChild.IsValid())
	{
		if (Orientation == Orient_Vertical)
		{
			switch (InNavigationEvent.GetNavigationType())
			{
			case EUINavigation::Up:
				FocusedChildDirection = -1;
				break;
			case EUINavigation::Down:
				FocusedChildDirection = 1;
				break;
			default:
				// If we don't handle this direction in our current orientation we can 
				// just allow the behavior of the boundary rule take over.
				return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
			}
		}
		else // Orient_Horizontal
		{
			switch (InNavigationEvent.GetNavigationType())
			{
			case EUINavigation::Left:
				FocusedChildDirection = -1;
				break;
			case EUINavigation::Right:
				FocusedChildDirection = 1;
				break;
			default:
				// If we don't handle this direction in our current orientation we can 
				// just allow the behavior of the boundary rule take over.
				return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
			}
		}

		// If the focused child index is in a valid range we know we can successfully focus
		// the new child we're moving focus to.
		if (FocusedChildDirection != 0)
		{
			TSharedPtr<SWidget> NextFocusableChild;

			// Search in the direction we need to move for the next focusable child of the scrollbox.
			for (int32 ChildIndex = FocusedChildIndex + FocusedChildDirection; ChildIndex >= 0 && ChildIndex < Children.Num(); ChildIndex += FocusedChildDirection)
			{
				TSharedPtr<SWidget> PossiblyFocusableChild = GetKeyboardFocusableWidget(Children[ChildIndex].GetWidget());
				if (PossiblyFocusableChild.IsValid())
				{
					NextFocusableChild = PossiblyFocusableChild;
					break;
				}
			}

			// If we found a focusable child, scroll to it, and shift focus.
			if (NextFocusableChild.IsValid())
			{
				InternalScrollDescendantIntoView(MyGeometry, NextFocusableChild, false, NavigationDestination, NavigationScrollPadding);
				return FNavigationReply::Explicit(NextFocusableChild);
			}
		}
	}

	return SCompoundWidget::OnNavigation(MyGeometry, InNavigationEvent);
}

FReply SScrollBoxEx::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const float ScrollByAmountScreen = GetScrollComponentFromVector(MouseEvent.GetCursorDelta());
	const float ScrollByAmountLocal = ScrollByAmountScreen / MyGeometry.Scale;

	if (MouseEvent.IsTouchEvent())
	{
		FReply Reply = FReply::Unhandled();

		if (!bTouchPanningCapture)
		{
			if (bFingerOwningTouchInteraction.IsSet() && MouseEvent.IsTouchEvent() && !HasMouseCapture())
			{
				PendingScrollTriggerAmount += ScrollByAmountScreen;

				if (FMath::Abs(PendingScrollTriggerAmount) > FSlateApplication::Get().GetDragTriggerDistance())
				{
					bTouchPanningCapture = true;
					ScrollBar->BeginScrolling();

					// The user has moved the list some amount; they are probably
					// trying to scroll. From now on, the list assumes the user is scrolling
					// until they lift their finger.
					Reply = FReply::Handled().CaptureMouse(AsShared());
				}
				else
				{
					Reply = FReply::Handled();
				}
			}
		}
		else
		{
			if (bFingerOwningTouchInteraction.IsSet() && HasMouseCaptureByUser(MouseEvent.GetUserIndex(), MouseEvent.GetPointerIndex()))
			{
				LastScrollTime = FSlateApplication::Get().GetCurrentTime();
				InertialScrollManager.AddScrollSample(-ScrollByAmountScreen, FSlateApplication::Get().GetCurrentTime());
				ScrollBy(MyGeometry, -ScrollByAmountLocal, EAllowOverscroll::Yes, false);

				Reply = FReply::Handled();
			}
		}

		return Reply;
	}
	else
	{
		if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && bAllowsRightClickDragScrolling)
		{
			// If scrolling with the right mouse button, we need to remember how much we scrolled.
			// If we did not scroll at all, we will bring up the context menu when the mouse is released.
			AmountScrolledWhileRightMouseDown += FMath::Abs(ScrollByAmountScreen);

			// Has the mouse moved far enough with the right mouse button held down to start capturing
			// the mouse and dragging the view?
			if (IsRightClickScrolling())
			{
				InertialScrollManager.AddScrollSample(-ScrollByAmountScreen, FPlatformTime::Seconds());
				const bool bDidScroll = ScrollBy(MyGeometry, -ScrollByAmountLocal, AllowOverscroll, false);

				FReply Reply = FReply::Handled();

				// Capture the mouse if we need to
				if (HasMouseCapture() == false)
				{
					Reply.CaptureMouse(AsShared()).UseHighPrecisionMouseMovement(AsShared());
					SoftwareCursorPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
					bShowSoftwareCursor = true;
				}

				// Check if the mouse has moved.
				if (bDidScroll)
				{
					SetScrollComponentOnVector(SoftwareCursorPosition, GetScrollComponentFromVector(SoftwareCursorPosition) + GetScrollComponentFromVector(MouseEvent.GetCursorDelta()));
				}

				return Reply;
			}
		}
	}

	return FReply::Unhandled();
}

FReply SScrollBoxEx::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if ((ScrollBar->IsNeeded() && ConsumeMouseWheel != EConsumeMouseWheel::Never) || ConsumeMouseWheel == EConsumeMouseWheel::Always)
	{
		// Make sure scroll velocity is cleared so it doesn't fight with the mouse wheel input
		InertialScrollManager.ClearScrollVelocity();

		const bool bScrollWasHandled = ScrollBy(MyGeometry, -MouseEvent.GetWheelDelta() * GetGlobalScrollAmount() * WheelScrollMultiplier, EAllowOverscroll::No, bAnimateWheelScrolling);

		if (bScrollWasHandled && !bIsScrollingActiveTimerRegistered)
		{
			// Register the active timer to handle the inertial scrolling
			CachedGeometry = MyGeometry;
			BeginInertialScrolling();
		}

		return bScrollWasHandled ? FReply::Handled() : FReply::Unhandled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SScrollBoxEx::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bAllowsRightClickDragScrolling)
	{
		if (!bIsScrollingActiveTimerRegistered && IsRightClickScrolling())
		{
			// Register the active timer to handle the inertial scrolling
			CachedGeometry = MyGeometry;
			BeginInertialScrolling();
		}

		AmountScrolledWhileRightMouseDown = 0;

		Invalidate(EInvalidateWidget::Layout);

		FReply Reply = FReply::Handled().ReleaseMouseCapture();
		bShowSoftwareCursor = false;

		// If we have mouse capture, snap the mouse back to the closest location that is within the panel's bounds
		if (HasMouseCapture())
		{
			FSlateRect PanelScreenSpaceRect = MyGeometry.GetLayoutBoundingRect();
			FVector2D CursorPosition = MyGeometry.LocalToAbsolute(SoftwareCursorPosition);

			FIntPoint BestPositionInPanel(
				FMath::RoundToInt(FMath::Clamp(CursorPosition.X, PanelScreenSpaceRect.Left, PanelScreenSpaceRect.Right)),
				FMath::RoundToInt(FMath::Clamp(CursorPosition.Y, PanelScreenSpaceRect.Top, PanelScreenSpaceRect.Bottom))
			);

			Reply.SetMousePos(BestPositionInPanel);
		}

		return Reply;
	}

	return FReply::Unhandled();
}

FReply SScrollBoxEx::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	CachedGeometry = MyGeometry;

	if (HasMouseCaptureByUser(InTouchEvent.GetUserIndex(), InTouchEvent.GetPointerIndex()))
	{
		ScrollBar->EndScrolling();
		Invalidate(EInvalidateWidget::Layout);

		BeginInertialScrolling();

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SScrollBoxEx::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	if (ScrollWhenFocusChanges != EScrollWhenFocusChanges::NoScroll)
	{
		if (NewWidgetPath.IsValid() && NewWidgetPath.ContainsWidget(this))
		{
			ScrollDescendantIntoView(NewWidgetPath.GetLastWidget(), ScrollWhenFocusChanges == EScrollWhenFocusChanges::AnimatedScroll ? true : false, NavigationDestination, NavigationScrollPadding);
		}
	}
}

bool SScrollBoxEx::CanUseInertialScroll(float ScrollAmount) const
{
	const auto CurrentOverscroll = Overscroll.GetOverscroll(CachedGeometry);

	// We allow sampling for the inertial scroll if we are not in the overscroll region,
	// Or if we are scrolling outwards of the overscroll region
	return CurrentOverscroll == 0.f || FMath::Sign(CurrentOverscroll) != FMath::Sign(ScrollAmount);
}

bool SScrollBoxEx::ScrollBy(const FGeometry& AllottedGeometry, float LocalScrollAmount, EAllowOverscroll Overscrolling, bool InAnimateScroll)
{
	Invalidate(EInvalidateWidget::LayoutAndVolatility);

	bAnimateScroll = InAnimateScroll;

	const float ContentSize = GetScrollComponentFromVector(ScrollPanel->GetDesiredSize());
	const FGeometry ScrollPanelGeometry = FindChildGeometry(AllottedGeometry, ScrollPanel.ToSharedRef());

	const float PreviousScrollOffset = DesiredScrollOffset;

	if (LocalScrollAmount != 0)
	{
		const float ScrollMin = 0.0f;
		const float ScrollMax = FMath::Max(ContentSize - GetScrollComponentFromVector(ScrollPanelGeometry.GetLocalSize()), 0.0f);

		if (AllowOverscroll == EAllowOverscroll::Yes && Overscrolling == EAllowOverscroll::Yes && Overscroll.ShouldApplyOverscroll(DesiredScrollOffset == 0, DesiredScrollOffset == ScrollMax, LocalScrollAmount))
		{
			Overscroll.ScrollBy(AllottedGeometry, LocalScrollAmount);
		}
		else
		{
			DesiredScrollOffset = FMath::Clamp(DesiredScrollOffset + LocalScrollAmount, ScrollMin, ScrollMax);
		}
	}

	OnUserScrolled.ExecuteIfBound(DesiredScrollOffset);

	return ConsumeMouseWheel == EConsumeMouseWheel::Always || DesiredScrollOffset != PreviousScrollOffset;
}

void SScrollBoxEx::BeginInertialScrolling()
{
	if (!UpdateInertialScrollHandle.IsValid())
	{
		bIsScrolling = true;
		bIsScrollingActiveTimerRegistered = true;
		UpdateInertialScrollHandle = RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SScrollBoxEx::UpdateInertialScroll));
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

EActiveTimerReturnType SScrollBoxEx::UpdateInertialScroll(double InCurrentTime, float InDeltaTime)
{
	bool bKeepTicking = bIsScrolling;

	if (bIsScrolling)
	{
		InertialScrollManager.UpdateScrollVelocity(InDeltaTime);
		const float ScrollVelocityLocal = InertialScrollManager.GetScrollVelocity() / CachedGeometry.Scale;

		if (ScrollVelocityLocal != 0.f)
		{
			if (CanUseInertialScroll(ScrollVelocityLocal))
			{
				bKeepTicking = true;
				ScrollBy(CachedGeometry, ScrollVelocityLocal * InDeltaTime, AllowOverscroll, false);
			}
			else
			{
				InertialScrollManager.ClearScrollVelocity();
			}
		}
	}

	if (AllowOverscroll == EAllowOverscroll::Yes)
	{
		// If we are currently in overscroll, the list will need refreshing.
		// Do this before UpdateOverscroll, as that could cause GetOverscroll() to be 0
		if (Overscroll.GetOverscroll(CachedGeometry) != 0.0f)
		{
			bKeepTicking = true;
		}

		Overscroll.UpdateOverscroll(InDeltaTime);
	}

	TickScrollDelta = 0.f;

	if (!bKeepTicking)
	{
		bIsScrolling = false;
		bIsScrollingActiveTimerRegistered = false;
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
		UpdateInertialScrollHandle.Reset();
		OnScrollEnd.ExecuteIfBound(DesiredScrollOffset);
	}

	return bKeepTicking ? EActiveTimerReturnType::Continue : EActiveTimerReturnType::Stop;
}

#undef LOCTEXT_NAMESPACE
#endif