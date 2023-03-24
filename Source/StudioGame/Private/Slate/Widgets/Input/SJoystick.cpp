// Copyright 2022 CQUnreal. All Rights Reserved.

#include "Widgets/Input/SJoystick.h"
#include "Launch/Resources/Version.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"

#if ENGINE_MAJOR_VERSION > 4
const static FPlatformUserId PlatformUserId = FPlatformUserId::CreateFromInternalId(0);
const static FInputDeviceId InputDeviceId = FInputDeviceId::CreateFromInternalId(0);
#else
const static int32 JoystickControllerId = 0;
#endif

void SJoystick::Construct(const FArguments& InArgs)
{
	SetCanTick(true);

	bVisible = true;
	PointerIndex = -1;
	PositionOffset = FVector2D::ZeroVector;
	LastMovedOffset = FVector2D::ZeroVector;
	ThumbPosition = FVector2D::ZeroVector;

	PCOwner = InArgs._PCOwner;

	ActionKey = InArgs._ActionKey;
	XAxis = InArgs._XAxis;
	YAxis = InArgs._YAxis;
	InputMode = InArgs._InputMode;
	bFixed = InArgs._bFixed;

	XAxisRate = InArgs._XAxisRate;
	YAxisRate = InArgs._YAxisRate;

	ColorAndOpacity = InArgs._ColorAndOpacity;

	SetJoystickStyle(InArgs._JoystickStyle);
}

int32 SJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	FPaintGeometry DefaultPaintGeometry = AllottedGeometry.ToPaintGeometry();
	
	const FVector2D DrawPosition = FVector2D(DefaultPaintGeometry.DrawPosition);
	const FVector2D LocalSize = DefaultPaintGeometry.GetLocalSize();
	const float DrawScale = DefaultPaintGeometry.DrawScale;

	if ((VisualImage != nullptr) && (VisualImage->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * VisualImage->GetTint(InWidgetStyle));

		FPaintGeometry PaintGeometry(DrawPosition + (DrawCenter - VisualSize * 0.5f) * DrawScale, VisualSize * DrawScale, DrawScale);

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, VisualImage, DrawEffects, FinalColorAndOpacity);
	}

	if ((ThumbImage != nullptr) && (ThumbImage->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const FVector2D CenterPosition = DrawPosition + LocalSize * 0.5f * DrawScale;
		const FVector2D ThumbDrawPosition = CenterPosition - ThumbImage->ImageSize * DrawScale * 0.5f + ThumbPosition;

		FPaintGeometry PaintGeometry(DrawPosition + (DrawCenter - ThumbSize * 0.5f) * DrawScale + ThumbPosition, ThumbSize * DrawScale, DrawScale);

		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * ThumbImage->GetTint(InWidgetStyle));

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeometry, ThumbImage, DrawEffects, FinalColorAndOpacity);
	}

	return LayerId;
}

FReply SJoystick::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bMouseButtonDown = true;

	PointerIndex = MouseEvent.GetPointerIndex();

	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	LastMovedOffset = LocalCoord;
	StartedPosition = LocalCoord;
	if (bFixed)
	{
		DrawCenter = Center;
	}
	else
	{
		DrawCenter = LocalCoord;
	}

	if (ActionKey.IsValid() && PCOwner != nullptr)
	{
		#if ENGINE_MAJOR_VERSION > 4
		PCOwner->InputKey(FInputKeyParams(ActionKey, EInputEvent::IE_Pressed, 0.0, true));
		#else
		PCOwner->InputKey(ActionKey, EInputEvent::IE_Pressed, 0.f, true);
		#endif
	}

	return FReply::Handled().CaptureMouse(SharedThis(this));
}

FReply SJoystick::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (PointerIndex == MouseEvent.GetPointerIndex())
	{
		FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		PositionOffset = (LocalCoord - LastMovedOffset);
		MovedOffset = (LocalCoord - StartedPosition);
		LastMovedOffset = LocalCoord;

		// figure out position around started postion
		FPaintGeometry DefaultPaintGeometry = MyGeometry.ToPaintGeometry();
		FVector2D Offset = MovedOffset * 0.5f;
		float DistanceToCenter = FMath::Sqrt(Offset.X * Offset.X + Offset.Y * Offset.Y);
		FVector2D VisualRange = VisualSize * 0.5f * DefaultPaintGeometry.DrawScale;
		float VisualRadius = FMath::Sqrt(VisualRange.X * VisualRange.X + VisualRange.Y * VisualRange.Y);
		if (DistanceToCenter > VisualRadius)
		{
			ThumbPosition = (VisualRadius / DistanceToCenter) * Offset;
		}
		else
		{
			ThumbPosition = Offset;
		}

		if (InputMode == EJoystickInputMode::Slide)
		{
			#if ENGINE_MAJOR_VERSION > 4
			FSlateApplication::Get().OnControllerAnalog(GetGamepadXAxis(), PlatformUserId, InputDeviceId, PositionOffset.X * XAxisRate);
			FSlateApplication::Get().OnControllerAnalog(GetGamepadYAxis(), PlatformUserId, InputDeviceId, -PositionOffset.Y * YAxisRate);
			#else
			FSlateApplication::Get().OnControllerAnalog(GetGamepadXAxis(), JoystickControllerId, PositionOffset.X * XAxisRate);
			FSlateApplication::Get().OnControllerAnalog(GetGamepadYAxis(), JoystickControllerId, -PositionOffset.Y * YAxisRate);
			#endif

			PositionOffset = FVector2D::ZeroVector;
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SJoystick::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetPointerIndex() == PointerIndex)
	{
		bMouseButtonDown = false;

		PointerIndex = INDEX_NONE;

		MovedOffset = FVector2D::ZeroVector;
		ThumbPosition = FVector2D::ZeroVector;

		if (!bFixed)
		{
			DrawCenter = Center;
		}

		if (ActionKey.IsValid() && PCOwner != nullptr)
		{
			#if ENGINE_MAJOR_VERSION > 4
			PCOwner->InputKey(FInputKeyParams(ActionKey, EInputEvent::IE_Released, 0.0, true));
			#else
			PCOwner->InputKey(ActionKey, EInputEvent::IE_Released, 0.f, true);
			#endif
		}

		#if ENGINE_MAJOR_VERSION > 4
		FSlateApplication::Get().OnControllerButtonReleased(GetGamepadXAxis(), PlatformUserId, InputDeviceId, false);
		FSlateApplication::Get().OnControllerButtonReleased(GetGamepadYAxis(), PlatformUserId, InputDeviceId, false);
		#else
		FSlateApplication::Get().OnControllerButtonReleased(GetGamepadXAxis(), JoystickControllerId, false);
		FSlateApplication::Get().OnControllerButtonReleased(GetGamepadYAxis(), JoystickControllerId, false);
		#endif

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}

void SJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// reset thumb postion
	if (PointerIndex == INDEX_NONE && !ThumbPosition.IsNearlyZero())
	{
		ThumbPosition = FMath::Vector2DInterpTo(ThumbPosition, FVector2D::ZeroVector, InDeltaTime, 25.f);
	}

	// now pass the fake move events to the game
	if (bMouseButtonDown && InputMode == EJoystickInputMode::Joystick && PointerIndex != INDEX_NONE)
	{
		FVector2D InputAxis = ThumbPosition.GetSafeNormal();

#if ENGINE_MAJOR_VERSION > 4
		FSlateApplication::Get().OnControllerAnalog(GetGamepadXAxis(), PlatformUserId, InputDeviceId, InputAxis.X * XAxisRate);
		FSlateApplication::Get().OnControllerAnalog(GetGamepadYAxis(), PlatformUserId, InputDeviceId, -InputAxis.Y * YAxisRate);
#else
		FSlateApplication::Get().OnControllerAnalog(GetGamepadXAxis(), JoystickControllerId, InputAxis.X * XAxisRate);
		FSlateApplication::Get().OnControllerAnalog(GetGamepadYAxis(), JoystickControllerId, -InputAxis.Y * YAxisRate);
#endif
	}
}

FVector2D SJoystick::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100, 100);
}

void SJoystick::SetJoystickStyle(const FJoystickStyle* JoystickStyle)
{
	Style = JoystickStyle;

	VisualImage = &JoystickStyle->VisualBrush;
	ThumbImage = &JoystickStyle->ThumbBrush;
	Center = JoystickStyle->Center;
	DrawCenter = JoystickStyle->Center;
	VisualSize = JoystickStyle->VisualSize;
	ThumbSize = JoystickStyle->ThumbSize;
}

void SJoystick::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	SetAttribute(ColorAndOpacity, InColorAndOpacity, EInvalidateWidgetReason::Paint);
}

void SJoystick::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	SetColorAndOpacity(TAttribute<FSlateColor>(InColorAndOpacity));
}

void SJoystick::SetXAxisRate(const float InValue)
{
	XAxisRate = InValue;
}

void SJoystick::SetYAxisRate(const float InValue)
{
	YAxisRate = InValue;
}

void SJoystick::SetFixed(const bool InValue)
{
	bFixed = InValue;
}

FVector2D SJoystick::GetMovedOffset() const
{
	return MovedOffset;
}

const FGamepadKeyNames::Type SJoystick::GetGamepadXAxis() const
{
	return XAxis.IsValid() ? XAxis.GetFName() : FGamepadKeyNames::LeftAnalogX;
}

const FGamepadKeyNames::Type SJoystick::GetGamepadYAxis() const
{
	return YAxis.IsValid() ? YAxis.GetFName() : FGamepadKeyNames::LeftAnalogY;
}