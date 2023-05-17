#include "UMG/Components/ScrollBoxEx.h"
#include "UMG/Components/ScrollBoxSlotEx.h"

UScrollBoxEx::UScrollBoxEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UScrollBoxEx::GetScrollOffsetOfEndEx() const
{
#if ENGINE_MAJOR_VERSION >= 5
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffsetOfEnd();
	}
#endif
	return 0;

}

float UScrollBoxEx::GetScrollOffsetEx() const
{
#if ENGINE_MAJOR_VERSION >= 5
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffset();
	}
#endif
	return 0;

}

void UScrollBoxEx::ScrollWidgetIntoViewEx(UWidget* WidgetToFind, bool AnimateScroll, EDescendantScrollDestination InScrollDestination, float Padding)
{
#if ENGINE_MAJOR_VERSION >= 5
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MyScrollBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to scroll which is necessary to avoid warnings/ensures 
		//       when we request to scroll to a widget and later remove that widget!
		MyScrollBox->ScrollDescendantIntoView(SlateWidgetToFind, AnimateScroll, InScrollDestination, Padding);
	}
#endif
}

#if ENGINE_MAJOR_VERSION >= 5

void UScrollBoxEx::SynchronizeProperties()
{
	UWidget::SynchronizeProperties();

	MyScrollBox->SetScrollOffset(DesiredScrollOffset);
	MyScrollBox->SetOrientation(Orientation);
	MyScrollBox->SetScrollBarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(ScrollBarVisibility));
	MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	MyScrollBox->SetScrollBarPadding(ScrollbarPadding);
	MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	MyScrollBox->SetScrollBarTrackAlwaysVisible(AlwaysShowScrollbarTrack);
	MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	MyScrollBox->SetScrollBarRightClickDragAllowed(bAllowRightClickDragScrolling);
	MyScrollBox->SetConsumeMouseWheel(ConsumeMouseWheel);
	MyScrollBox->SetAnimateWheelScrolling(bAnimateWheelScrolling);
	MyScrollBox->SetWheelScrollMultiplier(WheelScrollMultiplier);
}

void UScrollBoxEx::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyScrollBox.Reset();
}

TSharedRef<SWidget>UScrollBoxEx::RebuildWidget()
{
	MyScrollBox = SNew(SScrollBoxEx)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.ScrollWhenFocusChanges(ScrollWhenFocusChanges)
		.BackPadScrolling(BackPadScrolling)
		.FrontPadScrolling(FrontPadScrolling)
		.AnimateWheelScrolling(bAnimateWheelScrolling)
		.WheelScrollMultiplier(WheelScrollMultiplier)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled))
		.OnScrollEnd(BIND_UOBJECT_DELEGATE(FOnScrollEnd, SlateHandleScrollEnd));

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UScrollBoxSlotEx* TypedSlot = Cast<UScrollBoxSlotEx>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
		}
	}

	return MyScrollBox.ToSharedRef();
}

void UScrollBoxEx::SlateHandleScrollEnd(float CurrentOffset)
{
	OnScrollEnd.Broadcast(CurrentOffset);
}

UClass* UScrollBoxEx::GetSlotClass() const
{
	return UScrollBoxSlotEx::StaticClass();
}

void UScrollBoxEx::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyScrollBox.IsValid())
	{
		CastChecked<UScrollBoxSlotEx>(InSlot)->BuildSlot(MyScrollBox.ToSharedRef());
	}
}

void UScrollBoxEx::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyScrollBox.IsValid() && InSlot->Content)
	{
		const TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyScrollBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}
#endif