// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/Input/SDoubleSlider.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"
#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateAccessibleWidgets.h"
#endif

#if ENGINE_MAJOR_VERSION >= 5
SLATE_IMPLEMENT_WIDGET(SDoubleSlider)
void SDoubleSlider::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Value", ValueSlateAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "IndentHandle", IndentHandleSlateAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Locked", LockedSlateAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "SliderBarColor", SliderBarColorSlateAttribute, EInvalidateWidgetReason::Paint);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "SliderHandleColor", SliderHandleColorSlateAttribute, EInvalidateWidgetReason::Paint);
}

SDoubleSlider::SDoubleSlider()
	: Style(nullptr)
	, PressedScreenSpaceTouchDownPosition(FVector2D(0, 0))
	, ValueSlateAttribute(*this, FInputRange(0.f, 1.f))
	, IndentHandleSlateAttribute(*this, true)
	, LockedSlateAttribute(*this, false)
	, SliderBarColorSlateAttribute(*this, FLinearColor::White)
	, SliderHandleColorSlateAttribute(*this, FLinearColor::White)
{
#if WITH_ACCESSIBILITY
	AccessibleBehavior = EAccessibleBehavior::Summary;
	bCanChildrenBeAccessible = false;
#endif
}
#else
SDoubleSlider::SDoubleSlider()
{
#if WITH_ACCESSIBILITY
	AccessibleBehavior = EAccessibleBehavior::Summary;
	bCanChildrenBeAccessible = false;
#endif
}
#endif

void SDoubleSlider::Construct( const SDoubleSlider::FArguments& InDeclaration )
{
	check(InDeclaration._Style);

	Style = InDeclaration._Style;

#if ENGINE_MAJOR_VERSION >= 5
	IndentHandleSlateAttribute.Assign(*this, InDeclaration._IndentHandle);
	LockedSlateAttribute.Assign(*this, InDeclaration._Locked);
	ValueSlateAttribute.Assign(*this, InDeclaration._Value);
	SliderBarColorSlateAttribute.Assign(*this, InDeclaration._SliderBarColor);
	SliderHandleColorSlateAttribute.Assign(*this, InDeclaration._SliderHandleColor);
#else
	IndentHandleSlateAttribute = InDeclaration._IndentHandle;
	LockedSlateAttribute = InDeclaration._Locked;
	ValueSlateAttribute = InDeclaration._Value;
	SliderBarColorSlateAttribute = InDeclaration._SliderBarColor;
	SliderHandleColorSlateAttribute = InDeclaration._SliderHandleColor;
#endif
	bMouseUsesStep = InDeclaration._MouseUsesStep;
	bRequiresControllerLock = InDeclaration._RequiresControllerLock;
	Orientation = InDeclaration._Orientation;
	StepSize = InDeclaration._StepSize;
	DValue = InDeclaration._DValue;
	MinValue = InDeclaration._MinValue;
	MaxValue = InDeclaration._MaxValue;
	bIsFocusable = InDeclaration._IsFocusable;
	OnMouseCaptureBegin = InDeclaration._OnMouseCaptureBegin;
	OnMouseCaptureEnd = InDeclaration._OnMouseCaptureEnd;
	OnControllerCaptureBegin = InDeclaration._OnControllerCaptureBegin;
	OnControllerCaptureEnd = InDeclaration._OnControllerCaptureEnd;
	OnValueChanged = InDeclaration._OnValueChanged;

	bControllerInputCaptured = false;
}

int32 SDoubleSlider::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
	// we draw the slider like a horizontal slider regardless of the orientation, and apply a render transform to make it display correctly.
	// However, the AllottedGeometry is computed as it will be rendered, so we have to use the "horizontal orientation" when doing drawing computations.
	const float AllottedWidth = Orientation == Orient_Horizontal ? AllottedGeometry.GetLocalSize().X : AllottedGeometry.GetLocalSize().Y;
	const float AllottedHeight = Orientation == Orient_Horizontal ? AllottedGeometry.GetLocalSize().Y : AllottedGeometry.GetLocalSize().X;

	float HandleRotation;
	FVector2D MinHandleTopLeftPoint;
	FVector2D MaxHandleTopLeftPoint;
	FVector2D SliderStartPoint;
	FVector2D SliderEndPoint;

	// calculate slider geometry as if it's a horizontal slider (we'll rotate it later if it's vertical)
	const FVector2D HandleSize = GetThumbImage()->ImageSize;
	const FVector2D HalfHandleSize = 0.5f * HandleSize;
	const float Indentation = IndentHandleSlateAttribute.Get() ? HandleSize.X : 0.0f;

	// We clamp to make sure that the slider cannot go out of the slider Length.
	const FInputRange NormalizedValue = GetNormalizedValue();
	const float MinSliderPercent = FMath::Clamp(NormalizedValue.Min, 0.0f, 1.0f);
	const float MaxSliderPercent = FMath::Clamp(NormalizedValue.Max, 0.0f, 1.0f);
	const float SliderLength = AllottedWidth - (Indentation + HandleSize.X);
	const float MinSliderHandleOffset = MinSliderPercent * SliderLength;
	const float MaxSliderHandleOffset = MaxSliderPercent * SliderLength;
	const float SliderY = 0.5f * AllottedHeight;

	HandleRotation = 0.0f;
	MinHandleTopLeftPoint = FVector2D(MinSliderHandleOffset + (0.5f * Indentation), SliderY - HalfHandleSize.Y);
	MaxHandleTopLeftPoint = FVector2D(MaxSliderHandleOffset + (0.5f * Indentation), SliderY - HalfHandleSize.Y);

	SliderStartPoint = FVector2D(HalfHandleSize.X, SliderY);
	SliderEndPoint = FVector2D(AllottedWidth - HalfHandleSize.X, SliderY);

	FGeometry SliderGeometry = AllottedGeometry;
	
	// rotate the slider 90deg if it's vertical. The 0 side goes on the bottom, the 1 side on the top.
	if (Orientation == Orient_Vertical)
	{
		// Do this by translating along -X by the width of the geometry, then rotating 90 degreess CCW (left-hand coords)
		FSlateRenderTransform SlateRenderTransform = TransformCast<FSlateRenderTransform>(Concatenate(Inverse(FVector2D(AllottedWidth, 0)), FQuat2D(FMath::DegreesToRadians(-90.0f))));
		// create a child geometry matching this one, but with the render transform.
		SliderGeometry = AllottedGeometry.MakeChild(
			FVector2D(AllottedWidth, AllottedHeight), 
			FSlateLayoutTransform(), 
			SlateRenderTransform, FVector2D::ZeroVector);
	}

	const bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	// draw slider bar
	auto BarTopLeft = FVector2D(SliderStartPoint.X, SliderStartPoint.Y - Style->BarThickness * 0.5f);
	auto BarSize = FVector2D(SliderEndPoint.X - SliderStartPoint.X, Style->BarThickness);
	auto BarImage = GetBarImage();
	auto ThumbImage = GetThumbImage();
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		SliderGeometry.ToPaintGeometry(BarTopLeft, BarSize),
		BarImage,
		DrawEffects,
		BarImage->GetTint(InWidgetStyle) * SliderBarColorSlateAttribute.Get().GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
		);

	++LayerId;

	// draw min slider thumb
	FSlateDrawElement::MakeBox( 
		OutDrawElements,
		LayerId,
		SliderGeometry.ToPaintGeometry(MinHandleTopLeftPoint, GetThumbImage()->ImageSize),
		ThumbImage,
		DrawEffects,
		ThumbImage->GetTint(InWidgetStyle) * SliderHandleColorSlateAttribute.Get().GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
	);

	// draw max slider thumb
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		SliderGeometry.ToPaintGeometry(MaxHandleTopLeftPoint, GetThumbImage()->ImageSize),
		ThumbImage,
		DrawEffects,
		ThumbImage->GetTint(InWidgetStyle) * SliderHandleColorSlateAttribute.Get().GetColor(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
	);

	return LayerId;
}

FVector2D SDoubleSlider::ComputeDesiredSize( float ) const
{
	static const FVector2D SDoubleSliderDesiredSize(16.0f, 16.0f);

	if ( Style == nullptr )
	{
		return SDoubleSliderDesiredSize;
	}

	const float Thickness = FMath::Max(Style->BarThickness, 
		FMath::Max(Style->NormalThumbImage.ImageSize.Y, Style->HoveredThumbImage.ImageSize.Y));

	if (Orientation == Orient_Vertical)
	{
		return FVector2D(Thickness, SDoubleSliderDesiredSize.Y);
	}

	return FVector2D(SDoubleSliderDesiredSize.X, Thickness);
}

void SDoubleSlider::SetStyle(const FDoubleSliderStyle* InStyle)
{
	Style = InStyle;
	Invalidate(EInvalidateWidgetReason::Layout);
}

bool SDoubleSlider::IsLocked() const
{
	return LockedSlateAttribute.Get();
}

bool SDoubleSlider::IsInteractable() const
{
	return IsEnabled() && !IsLocked() && SupportsKeyboardFocus();
}

bool SDoubleSlider::SupportsKeyboardFocus() const
{
	return bIsFocusable;
}

void SDoubleSlider::ResetControllerState()
{
	if (bControllerInputCaptured)
	{
		OnControllerCaptureEnd.ExecuteIfBound();
		bControllerInputCaptured = false;
	}
}

FReply SDoubleSlider::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();

	if (IsInteractable())
	{
		// The controller's bottom face button must be pressed once to begin manipulating the slider's value.
		// Navigation away from the widget is prevented until the button has been pressed again or focus is lost.
		// The value can be manipulated by using the game pad's directional arrows ( relative to slider orientation ).
		if (FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept && bRequiresControllerLock)
		{
			if (bControllerInputCaptured == false)
			{
				// Begin capturing controller input and allow user to modify the slider's value.
				bControllerInputCaptured = true;
				OnControllerCaptureBegin.ExecuteIfBound();
				Reply = FReply::Handled();
			}
			else
			{
				ResetControllerState();
				Reply = FReply::Handled();
			}
		}
		else
		{
			Reply = SLeafWidget::OnKeyDown(MyGeometry, InKeyEvent);
		}
	}
	else
	{
		Reply = SLeafWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

	return Reply;
}

FReply SDoubleSlider::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();
	if (bControllerInputCaptured)
	{
		Reply = FReply::Handled();
	}
	return Reply;
}

void SDoubleSlider::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (bControllerInputCaptured)
	{
		// Commit and reset state
		CommitValue(ValueSlateAttribute.Get());
		ResetControllerState();
	}
}

FReply SDoubleSlider::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if ((MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) && !IsLocked())
	{
		CachedCursor = GetCursor().Get(EMouseCursor::Default);
		OnMouseCaptureBegin.ExecuteIfBound();
		CommitValue(PositionToValue(MyGeometry, MouseEvent.GetLastScreenSpacePosition()));
		
		// Release capture for controller/keyboard when switching to mouse.
		ResetControllerState();
		
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}

	return FReply::Unhandled();
}

FReply SDoubleSlider::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if ((MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) && HasMouseCaptureByUser(MouseEvent.GetUserIndex(), MouseEvent.GetPointerIndex()))
	{
		SetCursor(CachedCursor);
		
		// Release capture for controller/keyboard when switching to mouse.
		ResetControllerState();
		
		return FReply::Handled().ReleaseMouseCapture();	
	}

	return FReply::Unhandled();
}

void SDoubleSlider::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	OnMouseCaptureEnd.ExecuteIfBound();
	SLeafWidget::OnMouseCaptureLost(CaptureLostEvent);
}

FReply SDoubleSlider::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if (HasMouseCaptureByUser(MouseEvent.GetUserIndex(), MouseEvent.GetPointerIndex()) && !IsLocked())
	{
		SetCursor((Orientation == Orient_Horizontal) ? EMouseCursor::ResizeLeftRight : EMouseCursor::ResizeUpDown);
		CommitValue(PositionToValue(MyGeometry, MouseEvent.GetLastScreenSpacePosition()));
		
		// Release capture for controller/keyboard when switching to mouse
		ResetControllerState();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SDoubleSlider::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	if (!IsLocked())
	{
		// Release capture for controller/keyboard when switching to mouse.
		ResetControllerState();

		PressedScreenSpaceTouchDownPosition = InTouchEvent.GetScreenSpacePosition();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SDoubleSlider::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	if (HasMouseCaptureByUser(InTouchEvent.GetUserIndex(), InTouchEvent.GetPointerIndex()))
	{
		CommitValue(PositionToValue(MyGeometry, InTouchEvent.GetScreenSpacePosition()));

		// Release capture for controller/keyboard when switching to mouse
		ResetControllerState();

		return FReply::Handled();
	}
	else if (!HasMouseCapture())
	{
		if (FSlateApplication::Get().HasTraveledFarEnoughToTriggerDrag(InTouchEvent, PressedScreenSpaceTouchDownPosition, Orientation))
		{
			CachedCursor = GetCursor().Get(EMouseCursor::Default);
			OnMouseCaptureBegin.ExecuteIfBound();

			CommitValue(PositionToValue(MyGeometry, InTouchEvent.GetScreenSpacePosition()));

			// Release capture for controller/keyboard when switching to mouse
			ResetControllerState();

			return FReply::Handled().CaptureMouse(SharedThis(this));
		}
	}

	return FReply::Unhandled();
}

FReply SDoubleSlider::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	if (HasMouseCaptureByUser(InTouchEvent.GetUserIndex(), InTouchEvent.GetPointerIndex()))
	{
		SetCursor(CachedCursor);
		OnMouseCaptureEnd.ExecuteIfBound();

		CommitValue(PositionToValue(MyGeometry, InTouchEvent.GetScreenSpacePosition()));

		// Release capture for controller/keyboard when switching to mouse.
		ResetControllerState();

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SDoubleSlider::CommitValue(FInputRange NewValue)
{
	const FInputRange OldValue = GetValue();

#if ENGINE_MAJOR_VERSION >= 5
	if (!ValueSlateAttribute.IsBound(*this))
	{
		ValueSlateAttribute.Assign(*this, NewValue);
	}
#else
	if (!ValueSlateAttribute.IsBound())
	{
		ValueSlateAttribute = NewValue;
	}
#endif

	Invalidate(EInvalidateWidgetReason::Paint);

	OnValueChanged.ExecuteIfBound(NewValue);
}

FInputRange SDoubleSlider::PositionToValue( const FGeometry& MyGeometry, const FVector2D& AbsolutePosition )
{
	const FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(AbsolutePosition);

	float RelativeValue;
	float Denominator;
	// Only need X as we rotate the thumb image when rendering vertically
	const float Indentation = GetThumbImage()->ImageSize.X * (IndentHandleSlateAttribute.Get() ? 2.f : 1.f);
	const float HalfIndentation = 0.5f * Indentation;

	if (Orientation == Orient_Horizontal)
	{
		Denominator = MyGeometry.Size.X - Indentation;
		RelativeValue = (Denominator != 0.f) ? (LocalPosition.X - HalfIndentation) / Denominator : 0.f;
	}
	else
	{
		Denominator = MyGeometry.Size.Y - Indentation;
		// Inverse the calculation as top is 0 and bottom is 1
		RelativeValue = (Denominator != 0.f) ? ((MyGeometry.Size.Y - LocalPosition.Y) - HalfIndentation) / Denominator : 0.f;
	}

	RelativeValue = FMath::Clamp(RelativeValue, 0.0f, 1.0f) * (MaxValue - MinValue) + MinValue;

	FInputRange CurrentValue = ValueSlateAttribute.Get();
	float CurrentStepSize = StepSize.Get();
	if (FMath::Abs(RelativeValue - CurrentValue.Min) < FMath::Abs(RelativeValue - CurrentValue.Max))
	{
		CurrentValue.Min = FMath::Clamp(RelativeValue, MinValue, CurrentValue.Max - DValue);
	}
	else
	{
		CurrentValue.Max = FMath::Clamp(RelativeValue, CurrentValue.Min + DValue, MaxValue);
	}

	return CurrentValue;
}

const FSlateBrush* SDoubleSlider::GetBarImage() const
{
	if (!IsEnabled() || LockedSlateAttribute.Get())
	{
		return &Style->DisabledBarImage;
	}
	else if (IsHovered())
	{
		return &Style->HoveredBarImage;
	}
	else
	{
		return &Style->NormalBarImage;
	}
}

const FSlateBrush* SDoubleSlider::GetThumbImage() const
{
	if (!IsEnabled() || LockedSlateAttribute.Get())
	{
		return &Style->DisabledThumbImage;
	}
	else if (IsHovered())
	{
		return &Style->HoveredThumbImage;
	}
	else
	{
		return &Style->NormalThumbImage;
	}
}

FInputRange SDoubleSlider::GetValue() const
{
	return ValueSlateAttribute.Get();
}

FInputRange SDoubleSlider::GetNormalizedValue() const
{
	if (MaxValue == MinValue)
	{
		return FInputRange(0.f, 1.f);
	}
	else
	{
		const FInputRange CurrentValue = ValueSlateAttribute.Get();
		const float MDValue = MaxValue - MinValue;
		return FInputRange((CurrentValue.Min - MinValue) / MDValue, (CurrentValue.Max - MinValue) / MDValue);
	}
}

void SDoubleSlider::SetValue(TAttribute<FInputRange> InValueAttribute)
{
#if ENGINE_MAJOR_VERSION >= 5
	ValueSlateAttribute.Assign(*this, MoveTemp(InValueAttribute));
#else
	ValueSlateAttribute = MoveTemp(InValueAttribute);
#endif
}

void SDoubleSlider::SetDValue(float InDValue)
{
	DValue = InDValue;
}

void SDoubleSlider::SetMinAndMaxValues(float InMinValue, float InMaxValue)
{
	MinValue = InMinValue;
	MaxValue = InMaxValue;
	if (MinValue > MaxValue)
	{
		MaxValue = MinValue;
	}
}

void SDoubleSlider::SetIndentHandle(TAttribute<bool> InIndentHandle)
{
#if ENGINE_MAJOR_VERSION >= 5
	IndentHandleSlateAttribute.Assign(*this, MoveTemp(InIndentHandle));
#else
	IndentHandleSlateAttribute = MoveTemp(InIndentHandle);
#endif
}

void SDoubleSlider::SetLocked(TAttribute<bool> InLocked)
{
#if ENGINE_MAJOR_VERSION >= 5
	LockedSlateAttribute.Assign(*this, MoveTemp(InLocked));
#else
	LockedSlateAttribute = MoveTemp(InLocked);
#endif
}

void SDoubleSlider::SetOrientation(EOrientation InOrientation)
{
	if (Orientation != InOrientation)
	{
		Orientation = InOrientation;
		Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void SDoubleSlider::SetSliderBarColor(TAttribute<FSlateColor> InSliderBarColor)
{
#if ENGINE_MAJOR_VERSION >= 5
	SliderBarColorSlateAttribute.Assign(*this, MoveTemp(InSliderBarColor));
#else
	SliderBarColorSlateAttribute = MoveTemp(InSliderBarColor);
#endif
}

void SDoubleSlider::SetSliderHandleColor(TAttribute<FSlateColor> InSliderHandleColor)
{
#if ENGINE_MAJOR_VERSION >= 5
	SliderHandleColorSlateAttribute.Assign(*this, MoveTemp(InSliderHandleColor));
#else
	SliderHandleColorSlateAttribute= MoveTemp(InSliderHandleColor);
#endif
}

float SDoubleSlider::GetStepSize() const
{
	return StepSize.Get();
}

void SDoubleSlider::SetStepSize(TAttribute<float> InStepSize)
{
	StepSize = MoveTemp(InStepSize);
}

void SDoubleSlider::SetMouseUsesStep(bool MouseUsesStep)
{
	bMouseUsesStep = MouseUsesStep;
}

void SDoubleSlider::SetRequiresControllerLock(bool RequiresControllerLock)
{
	bRequiresControllerLock = RequiresControllerLock;
}

#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> SDoubleSlider::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleSlider(SharedThis(this)));
}
#endif
